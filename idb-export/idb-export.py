import re
import collections
import idautils

from functools import cached_property
from pathlib import Path

class IdbExportError(Exception):
    pass

class Datum:
    def __init__(self, ref):
        self.type = get_type(ref) or guess_type(ref)
        self.name = idaapi.get_name(ref)

        # TODO: handle mangled names (starting with '?')
        if self.name == None or self.name == '' or self.type == None or self.name[0] == '?':
            self.valid = False
            return

        self.valid = True
        if '[' in self.type: # array -> pointer
            self.type = self.type.replace('[', '(&' + self.name + ')[', 1)
            self.definition = f'{self.type} = * ((decltype(&{self.name})) {hex(ref)});'
        elif '*)(' in self.type: # function pointer -> reference to function pointer
            self.type = self.type.replace('*)', '*&' + self.name + ')',  1)
            self.type = self.type.replace('__hidden this', '__hidden this_')
            self.definition = f'{self.type} = *((decltype(&{self.name})) {hex(ref)});'
        else: # scalar value -> reference
            self.type = self.type + '& ' + self.name
            self.definition = f'{self.type} = * ((decltype(&{self.name})) {hex(ref)});'

        self.declaration = 'extern ' + self.type + ';'

class RawDefinition:
    def __init__(self, definition):
        self.definition = definition
        self.name = None

    def signature_with_name(self, _):
        return self.definition

class FunctionArgument:
    register_pattern = re.compile(r'@<(?P<register_name>[^>]*)>')

    def __init__(self, index, signature):
        self.index = index
        self.register = None

        signature = self._replace_register(signature)
        try:
            self.signature = Definition(signature)
        except:
            print(f'WARNING: Could not hande argument with signature "{signature}"')
            self.signature = RawDefinition(signature)

    def _replace_register(self, signature):
        return self.register_pattern.sub(self._initialize_register, signature)

    def _initialize_register(self, match):
        if match:
            self.register = match['register_name']

        return ''

    @cached_property
    def name(self):
        if not self.signature.name or re.match(r'a\d+', self.signature.name):
            return f'a{self.index + 1}'

        return self.normalize_arg_name(self.signature.name)

    @cached_property
    def signature_with_name(self):
        return self.signature.signature_with_name(self.name).replace('  ', ' ').replace('* ', '*')

    @staticmethod
    def normalize_arg_name(argument_name):
        if argument_name == 'this':
            # TODO: fix this mess
            return 'this_'

        if argument_name.endswith('size'):
            argument_name += '_'

        return argument_name

class Function:
    register_pattern = re.compile(r'@<(?P<register_name>[^>]*)>')

    def __init__(self, ref):
        self.ref = ref
        self.ref_type = get_type(ref) or guess_type(ref)
        self.ref_name = get_func_name(ref)
        self.skip = False

        try:
            next(idautils.XrefsTo(ref))
            self.is_used = True
        except StopIteration:
            self.is_used = False

        # TODO: handle properly
        if self.ref_type == None:
            # TODO: there might be a bug in GuessType
            # Use the hex rays python bindings in IDA 6.6
            self.skip = True
            return

        func_flags = get_func_attr(ref, FUNCATTR_FLAGS)
        is_lib = func_flags & FUNC_LIB
        if is_lib:
            self.skip = True
            return

    @cached_property
    def calling_convention(self):
       # Strip the register annotations from the type
       ref_type = self.register_pattern.sub('', self.ref_type)

       # TODO: proper implementation
       if '__cdecl' in ref_type:
           return 'cdecl'
       elif '__usercall' in ref_type:
           return 'usercall'
       elif '__userpurge' in ref_type:
           return 'userpurge'
       elif '__stdcall' in ref_type:
           return 'stdcall'
       elif '__thiscall' in ref_type:
           return 'thiscall'
       elif '__fastcall' in ref_type:
           return 'fastcall'
       else:
           return None

    @cached_property
    def name(self):
        if self.calling_convention == 'stdcall':
            return re.sub(r'\@\d+', '', self.ref_name)
        else:
            return self.ref_name

    @cached_property
    def has_return_value(self):
        return self.return_type != 'void'

    @cached_property
    def return_type(self):
        """
        Transforms the ref_type string (IDA ref type) that looks like:
          > void __userpurge(char a1@<al>, char a2@<cl>)

        to the function's return type
        """

        return re.search('^\s*((const\s+)?\w+(?:\s*\*)*)', self.ref_type).group(1).strip()

    @cached_property
    def return_register(self):
        if self.has_return_value:
            return self.register_pattern.search(self.ref_type).group('register_name')

    @cached_property
    def signature(self):
        all_args = ', '.join(arg.signature_with_name for arg in self.arguments)

        if self.calling_convention in {'usercall', 'userpurge'}:
            name_with_cc = self.name
        elif self.calling_convention == None:
            name_with_cc = f'(*{self.name})'
        else:
            name_with_cc = f'(__{self.calling_convention}*{self.name})'

        return f'{self.return_type} {name_with_cc}({all_args})'

    @cached_property
    def all_arguments(self):
        depth = 0

        i = len(self.ref_type) - 1
        while i >= 0:
            if self.ref_type[i] == ')':
                depth += 1
            if self.ref_type[i] == '(':
                depth -= 1
            if depth == 0:
                return self.ref_type[i + 1: -1]
            i -= 1
        raise IdbExportError('Trying to get args when none seem to be present: %s' % self.ref_type)

    @cached_property
    def arguments(self):
        """
        Transforms the self.all_arguments string that looks like:
        > int a1, int a2, int (__fastcall *a2)(_DWORD, _DWORD)@<ebx>

        to an array with arguments.
        (e.g. ['int a1', 'int a2', 'int (__fastcall *a2)(_DWORD, _DWORD)@<ebx>']

        NOTE: in case of function pointers it only works with one level of nesting
        """

        args = filter(None, re.findall(r'(?:[^\(\,]|(?:\([^\)]*\)))*', self.all_arguments))
        return [FunctionArgument(i, str.strip(arg)) for i, arg in enumerate(args)]

    full_regsiter = {
        'eax': 'eax',
        'ax': 'eax',
        'al': 'eax',
        'ah': 'eax',

        'ebx': 'ebx',
        'bx': 'ebx',
        'bl': 'ebx',
        'bh': 'ebx',

        'ecx': 'ecx',
        'cx': 'ecx',
        'cl': 'ecx',
        'ch': 'ecx',

        'edx': 'edx',
        'dx': 'edx',
        'dl': 'edx',
        'dh': 'edx',

        'esi': 'esi',
        'si': 'esi',
        'sil': 'esi',
        'sih': 'esi',

        'edi': 'edi',
        'di': 'edi',
        'dil': 'edi',
        'dih': 'edi',

        'ebp': 'ebp',
        'bp': 'ebp',

        'esp': 'esp',
        'sp': 'esp',
    }

    def get_usercall_wrapper(self):
        result = self.signature

        if self.return_type == '__int64':
            # TODO: handle properly
            return result + '{ throw "not implemented"; }\n'

        result += ' {\n'
        result += f'    int address = {hex(self.ref)};\n'
        if self.has_return_value:
            result += f'    {self.return_type} result_;\n'

        result += '    __asm {\n'

        zeroed_registers = set()
        if self.has_return_value:
            # TODO: only do this if part of the register s used for return value:
            zeroed_registers.add(self.full_regsiter[self.return_register])
            result += f'        xor {self.full_regsiter[self.return_register]}, {self.full_regsiter[self.return_register]}\n'

        for argument in self.arguments:
            if argument.register:
                full_register = self.full_regsiter[argument.register]
                if full_register not in zeroed_registers and argument.register != full_register:
                    zeroed_registers.add(full_register)
                    result += f'        xor {full_register}, {full_register}\n'

        stack_arguments_count = 0
        for arg in reversed(self.arguments):
            if arg.register in {'sil', 'dil'}:
                # TODO: handle 64 bit case
                # 32-bit assembly does not have the SIL/DIL registers
                result += f'        mov {arg.register[:2]}, word ptr {arg.name}\n'
            elif arg.register:
                result += f'        mov {arg.register}, {arg.name}\n'
            elif arg.signature_with_name == '...':
                pass # TODO: handle this
            else:
                stack_arguments_count += 1
                result += f'        push dword ptr {arg.name}\n'

        result += '        call address\n'
        if self.has_return_value:
            # TODO: use the proper register for the return value
            if self.return_type in {'char', 'u8', 'bool', 'Order', 'BYTE', 'EndgameState', 'Tech'}:
                result += '        mov result_, al\n'
            elif self.return_type in {'short', '__int16', 'u16', 'UnitType', 'GamePosition'}:
                result += '        mov result_, ax\n'
            else:
                result += '        mov result_, eax\n'

        if self.calling_convention == 'usercall' and stack_arguments_count != 0:
            # Clean up the stack
            # TODO: handle args larger than a single register
            # TODO: handle non 32-bit architectures
            result += f'        add esp, {stack_arguments_count * 4}\n'

        result += '    }\n'

        if self.has_return_value:
            result += '    return result_;\n'

        result += '}'
        return result

    def build_export_definition(self):
        if self.name.find('@') >= 0:
            raise IdbExportError(f'Function name contains invalid character: {self.name}')

        if self.calling_convention in {'usercall', 'userpurge'}:
            return self.get_usercall_wrapper()
        else:
            return f'DECL_FUNC({self.signature}, {self.name}, {hex(self.ref)});'

    def build_export_declaration(self):
        if self.calling_convention in {'usercall', 'userpurge'}:
            return self.signature + ';'
        else:
            return f'extern {self.signature};'

def get_code_refs(ea):
    refs = []
    ref = Rfirst(ea)

    # skip the first code ref
    # it is to the next instruction
    ref = Rnext(ea, ref)

    while ref != idc.BADADDR:
        refs.append(ref)
        ref = Rnext(ea, ref)

    return refs

def get_data_refs(ea):
    refs = []
    ref = Dfirst(ea)

    while ref != idc.BADADDR:
        refs.append(ref)
        ref = Dnext(ea, ref)

    return refs

def get_referred_funcs(ea):
    refs = get_code_refs(ea) + get_data_refs(ea)

    result = []
    for ref in refs:
        ref_func = get_func(ref)
        if ref_func and ref_func.start_ea == ref:
            result.append(ref)

    return result

TYPES_HEADER_TEMPLATE = """#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <stdio.h>
#include <Windows.h>
#include <ddraw.h>
#include <Mmreg.h>
#include <DSound.h>

#define __unaligned
#define __hidden
#define __far
#define __noreturn

// Some convenience macros to make partial accesses nicer
// first unsigned macros:
#undef LOBYTE
#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
#undef LOWORD
#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
#define LODWORD(x)  (*((_DWORD*)&(x)))  // low dword
#undef HIBYTE
#define HIBYTE(x)   (*((_BYTE*)&(x)+sizeof(x)-1))
#undef HIWORD
#define HIWORD(x)   (*((_WORD*)&(x)+sizeof(x)/2-1))
#define HIDWORD(x)  (*((_DWORD*)&(x)+1))
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)
#define BYTE3(x)   BYTEn(x,  3)
#define BYTE4(x)   BYTEn(x,  4)
#define BYTE5(x)   BYTEn(x,  5)
#define BYTE6(x)   BYTEn(x,  6)
#define BYTE7(x)   BYTEn(x,  7)
#define BYTE8(x)   BYTEn(x,  8)
#define BYTE9(x)   BYTEn(x,  9)
#define BYTE10(x)  BYTEn(x, 10)
#define BYTE11(x)  BYTEn(x, 11)
#define BYTE12(x)  BYTEn(x, 12)
#define BYTE13(x)  BYTEn(x, 13)
#define BYTE14(x)  BYTEn(x, 14)
#define BYTE15(x)  BYTEn(x, 15)
#define WORD1(x)   WORDn(x,  1)
#define WORD2(x)   WORDn(x,  2)         // third word of the object, unsigned
#define WORD3(x)   WORDn(x,  3)
#define WORD4(x)   WORDn(x,  4)
#define WORD5(x)   WORDn(x,  5)
#define WORD6(x)   WORDn(x,  6)
#define WORD7(x)   WORDn(x,  7)

// now signed macros (the same but with sign extension)
#define SLOBYTE(x)   (*((__int8*)&(x)))
#define SLOWORD(x)   (*((__int16*)&(x)))
#define SLODWORD(x)  (*((__int32*)&(x)))
#define SHIBYTE(x)   (*((__int8*)&(x)+1))
#define SHIWORD(x)   (*((__int16*)&(x)+1))
#define SHIDWORD(x)  (*((__int32*)&(x)+1))
#define SBYTEn(x, n)   (*((__int8*)&(x)+n))
#define SWORDn(x, n)   (*((__int16*)&(x)+n))
#define SBYTE1(x)   SBYTEn(x,  1)
#define SBYTE2(x)   SBYTEn(x,  2)
#define SBYTE3(x)   SBYTEn(x,  3)
#define SBYTE4(x)   SBYTEn(x,  4)
#define SBYTE5(x)   SBYTEn(x,  5)
#define SBYTE6(x)   SBYTEn(x,  6)
#define SBYTE7(x)   SBYTEn(x,  7)
#define SBYTE8(x)   SBYTEn(x,  8)
#define SBYTE9(x)   SBYTEn(x,  9)
#define SBYTE10(x)  SBYTEn(x, 10)
#define SBYTE11(x)  SBYTEn(x, 11)
#define SBYTE12(x)  SBYTEn(x, 12)
#define SBYTE13(x)  SBYTEn(x, 13)
#define SBYTE14(x)  SBYTEn(x, 14)
#define SBYTE15(x)  SBYTEn(x, 15)
#define SWORD1(x)   SWORDn(x,  1)
#define SWORD2(x)   SWORDn(x,  2)
#define SWORD3(x)   SWORDn(x,  3)
#define SWORD4(x)   SWORDn(x,  4)
#define SWORD5(x)   SWORDn(x,  5)
#define SWORD6(x)   SWORDn(x,  6)
#define SWORD7(x)   SWORDn(x,  7)

namespace {namespace}
{{
typedef __int8 _BYTE;
typedef __int16 _WORD;
typedef __int32 _DWORD;
typedef __int64 _QWORD;
typedef __int8 _TBYTE;

{declarations}

{definitions}
}}
"""

HEADER_TEMPLATE = """#pragma once

#include "types.h"

namespace {namespace}
{{
{declarations}
}}
"""

CPP_TEMPLATE = """#include "{executable_name}/types.h"

namespace {namespace}
{{
#define DECL_FUNC(decl, func, offset) decl = (decltype(func)) offset;

{definitions}
#undef DECL_FUNC
}}
"""

def export(root_dir, executable_name, namespace):
    root_dir = Path(root_dir)

    function_declarations = []
    function_definitions = []
    export_functions(function_declarations, function_definitions)

    data_declarations = []
    data_definitions = []
    export_data(data_declarations, data_definitions)

    type_declarations = []
    type_definitions = []
    export_types(type_declarations, type_definitions)

    src_directory = root_dir / 'src' / executable_name
    include_directory = root_dir / 'include' / executable_name

    src_directory.mkdir(parents=True, exist_ok=True)
    include_directory.mkdir(parents=True, exist_ok=True)

    with (src_directory / 'offsets.cpp').open('wt') as cpp_file:
        content = CPP_TEMPLATE.format(
            namespace=namespace,
            executable_name=executable_name,
            definitions = '\n'.join(function_definitions + data_definitions),
        )
        cpp_file.write(content)

    with (include_directory / 'offsets.h').open('wt') as header_file:
        content = HEADER_TEMPLATE.format(
            namespace=namespace,
            declarations = '\n'.join(function_declarations + data_declarations),
        )
        header_file.write(content)

    with (include_directory / 'types.h').open('wt') as types_header_file:
        content = TYPES_HEADER_TEMPLATE.format(
            namespace=namespace,
            declarations=''.join(type_declarations),
            definitions=''.join(type_definitions),
        )
        types_header_file.write(content)

    print('Exported %d function symbols' % len(function_declarations))
    print('Exported %d data symbols' % len(data_declarations))
    print('Exported %d types' % len(type_declarations))

def is_blacklisted(text):
    blacklisted_identifiers = [
        'ImmGetContext',
        'ImmSetCompositionStringA',
        'ImmReleaseContext',
        'ImmGetCompositionStringA',
        'ImmAssociateContext',
        'VerQueryValueA',
        'GetFileVersionInfoSizeA',
        'GetFileVersionInfoA',
        'RtlUnwind',
        '__int128',
        '__IMPORT_DESCRIPTOR',
        'FlsAlloc',
        'FlsGetValue',
        'FlsSetValue',
        'FlsFree',
        '_PVFV',
    ]

    for identifier in blacklisted_identifiers:
        if identifier in text:
            return True
    return False

def exportable_functions():
    for n in range(idaapi.get_segm_qty()):
        segment = idaapi.getnseg(n)
        if idaapi.get_segm_name(segment).startswith('.text'):
            for function_ea in Functions(segment.start_ea, segment.end_ea):
                function = Function(function_ea)
                if function.skip:
                    continue

                if is_blacklisted(function.name):
                    continue

                yield function

def export_functions(declarations, definitions):
    unused_functions = 0

    for function in exportable_functions():
        if not function.is_used:
            unused_functions += 1

        declarations.append(function.build_export_declaration())
        definitions.append(function.build_export_definition())

    print('Unused functions: %d' % unused_functions)

def exportable_data():
    for segment in ['.rdata', '.data']:
        data_segment = idaapi.get_segm_by_name(segment)

        ea = data_segment.start_ea
        while ea != idc.BADADDR:
            ea = next_head(ea, data_segment.end_ea)
            datum = Datum(ea)

            if not datum.valid:
                continue

            if is_blacklisted(datum.declaration):
                continue

            yield datum

def export_data(declarations, definitions):
    for datum in exportable_data():
        declarations.append(datum.declaration)
        definitions.append(datum.definition)

BLACKLISTED_TYPES = {
    'DWORD', '_DWORD',
    'RECT', 'POINT', '_GUID',
    '__int128', 'wchar_t',
    'tm', 'tagPALETTEENTRY', 'type_info',
    '_PMD', '_PVFV', '_RTTIBaseClassDescriptor',
    'HCURSOR', 'HICON', 'HMODULE', 'HINSTANCE', 'HWND',
    'WNDCLASSA', 'HBRUSH', 'WNDPROC', 'LPMEMORYSTATUS',
    '_MEMORYSTATUS', 'HANDLE',
    'tagMSG', 'LPMSG',
}

def is_type_blacklisted(type_ordinal):
    local_type_name = get_numbered_type_name(type_ordinal)
    if local_type_name == None:
        return False # TODO: determine when this happens
    if local_type_name in BLACKLISTED_TYPES:
        return True
    if local_type_name.startswith('IDirectDraw'):
        return True
    if local_type_name.endswith('_Ref_count_base'):
        return True
    if local_type_name.startswith('_SYSTEM_INFO'):
        return True
    if local_type_name.startswith('_OVERLAPPED'):
        return True
    if local_type_name == 'VirtualKey':
        return True
    return False

class Definition(object):
    def __new__(cls, definition):
        for definition_type in [SimpleDefinition, FunctionPointerDefinition, VarArgsDefinition]:
            if definition_type.type_pattern.match(definition):
                self = object.__new__(definition_type)
                self.definition = definition
                return self

        raise IdbExportError(f'Could not build definition for "{definition}"')

    def __repr__(self):
        return f"{self.__class__.__name__}('{self.definition}')"

class SimpleDefinition(Definition):
    type_pattern = re.compile(
        r'''
        ^
        (const|signed|unsigned|struct|union|class|enum|\s)*
        (?P<type>\w+(\s|const|\*)*)
        (?P<name>\w*)
        (
            (\[.*\])*
            |
            (\s*:\s*\d+)
        )
        $
        ''', re.VERBOSE)

    @cached_property
    def name(self):
        return self.type_pattern.match(self.definition).group('name')

    @cached_property
    def type(self):
        return self.type_pattern.match(self.definition).group('type').strip().replace(' *', '*').replace('*const', '*')

    @cached_property
    def types(self):
        return {self.type}

    def signature_with_name(self, name):
        if self.type == 'void':
            return 'void'

        return self.type_pattern.sub(lambda m: f"{self.definition[:m.start('name')]} {name}{self.definition[m.end('name'):]}", self.definition)

class FunctionPointerDefinition(Definition):
    type_pattern = re.compile(
        r'''
        ^
        (const|signed|unsigned|struct|union|class|enum|\s)*
        (?P<return_type>\w+(\s|const|\*)*)
        \(
            (\w+\s*)*
            \*
            \s*
            (?P<name>\w*)
        \)
        \(
            (?P<args>.*)
        \)
        $
        ''', re.VERBOSE)

    @cached_property
    def name(self):
        return self.type_pattern.match(self.definition).group('name')

    @cached_property
    def types(self):
        match = self.type_pattern.match(self.definition)

        types = {match.group('return_type').strip()}

        args = match.group('args')
        args = args.split(',') if len(args) > 0 else []
        for arg in args:
            types |= set(Definition(arg.strip()).types)

        return types

    def signature_with_name(self, name):
        return self.type_pattern.sub(lambda m: f"{self.definition[:m.start('name')]} {name}{self.definition[m.end('name'):]}", self.definition)

class VarArgsDefinition(Definition):
    type_pattern = re.compile(r'^\.\.\.$')

    @property
    def name(self):
        return None

    @property
    def types(self):
        {'va_list'}

    def signature_with_name(self, _):
        return '...'

class Type(object):
    body_pattern = re.compile('{.*};')
    base_type_pattern = re.compile(r'\s*:\s*(?P<base_type>(\w+\s*)+)')
    type_qualifier_pattern = re.compile(r'\b(const|unsigned|signed|struct|union|class|enum)\b')

    keyword_constructor = {}

    def __new__(cls, type_definition):
        type_definition_lines = type_definition.split('\n')
        if type_definition_lines[0].startswith('#pragma') and type_definition_lines[-1].startswith('#pragma'):
            type_prologue = type_definition_lines[0] + '\n'
            type_epilogue = type_definition_lines[-1] + '\n'
            type_definition = '\n'.join(type_definition_lines[1:-1])
        elif type_definition_lines[0].startswith('#pragma') or type_definition_lines[-1].startswith('#pragma'):
            raise IdbExportError('Expected pragma at both the start and end of type')
        else:
            type_prologue = ''
            type_epilogue = ''

        type_keyword = type_definition.split(' ')[0]
        constructor = cls.keyword_constructor.get(type_keyword)
        if constructor:
            self = object.__new__(constructor)
            self.prologue = type_prologue
            self.definition = type_definition + ';\n'
            self.epilogue = type_epilogue
            return self
        else:
            raise IdbExportError('key word not registered')

    @classmethod
    def register_keyword(cls, keyword, subclass):
        cls.keyword_constructor[keyword] = subclass

    @cached_property
    def definition_without_body(self):
        local_type_oneline = self.definition.replace('\n', '')
        local_type_no_body = self.body_pattern.sub(';\n', local_type_oneline)
        return self.base_type_pattern.sub('', local_type_no_body)

    @cached_property
    def base_types(self):
        oneline_definition = self.definition.replace('\n', '')
        definition_without_body = self.body_pattern.sub('', oneline_definition)
        base_type_match = self.base_type_pattern.search(definition_without_body)

        # TODO: handle multiple base types
        if base_type_match:
            return [base_type_match.group('base_type').strip()]
        else:
            return []

    @cached_property
    def name(self):
        definition_without_body = self.base_type_pattern.sub('', self.definition_without_body).strip()
        return definition_without_body.replace(';', '').split()[-1]

    @cached_property
    def size(self):
        return None

def keywords(*keywords):
    def keywords_inner(cls):
        for keyword in keywords:
            Type.register_keyword(keyword, cls)
    return keywords_inner

@keywords('enum')
class EnumType(Type):
    @cached_property
    def declaration(self):
        local_type_oneline = self.definition.replace('\n', '')
        return self.body_pattern.sub(';\n', local_type_oneline)

    @cached_property
    def dependencies(self):
        return set(self.base_types)

@keywords('struct', 'union', 'class')
class CompositionType(Type):
    fields_pattern = re.compile(r'{(.*)}', re.DOTALL)

    @cached_property
    def fields(self):
        body_fields = self.fields_pattern.findall(self.definition)
        body_fields = body_fields[0].split(';') if len(body_fields) > 0 else []
        body_fields = map(str.strip, body_fields)
        return filter(lambda field: field != '', body_fields)

    @classmethod
    def simplify_field(cls, field):
        field = cls.type_qualifier_pattern.sub('', field)
        field = re.sub(r'\s+', ' ', field)
        return field.strip()

    @cached_property
    def declaration(self):
        return self.definition_without_body

    @cached_property
    def dependencies(self):
        result = set(self.base_types)
        for field in self.fields:
            result |= Definition(field).types
        return result

    @cached_property
    def size(self):
        return get_struc_size(get_struc_id(self.name))

@keywords('typedef')
class TypedefType(Type):
    @cached_property
    def declaration(self):
        return ''

    @cached_property
    def dependencies(self):
        return self.definition_object.types

    @cached_property
    def name(self):
        return self.definition_object.name

    @cached_property
    def definition_object(self):
        return Definition(self.definition.replace('typedef ', '').strip().rstrip(';'))

def export_types(declarations, definitions):
    existing_type_names = set()
    local_types = collections.OrderedDict()

    for type_ordinal in range(1, get_ordinal_qty()):
        if is_type_blacklisted(type_ordinal):
            continue

        type_name = get_numbered_type_name(type_ordinal)

        if type_name in existing_type_names:
            continue
        else:
            existing_type_names.add(type_name)

        try:
            type_definition = GetLocalType(type_ordinal, PRTYPE_MULTI | PRTYPE_TYPE | PRTYPE_PRAGMA).strip()
            local_type = Type(type_definition)
            local_types[local_type.name] = local_type
        except IdbExportError as error:
            print(error)
        except:
            continue

    for local_type in sort_topologically(local_types):
        declarations.append(local_type.declaration)
        definitions.append('\n' + local_type.prologue + local_type.definition + local_type.epilogue)
        if local_type.size != None:
            size_check = f'static_assert(sizeof({local_type.name}) == {local_type.size}, "Incorrect size for type `{local_type.name}`. Expected: {local_type.size}");\n'
            definitions.append(size_check)


from collections import defaultdict
from itertools import takewhile, count

def sort_topologically(local_types):
    graph = collections.OrderedDict()
    levels_by_name = collections.OrderedDict()
    names_by_level = collections.OrderedDict()

    for local_type in local_types.values():
        graph[local_type.name] = []
        for dependency in local_type.dependencies:
            dependency_type = dependency.replace('*', '')
            if dependency_type not in local_types:
                continue # There is no definition on depend on

            if '*' in dependency and local_types[dependency_type].declaration != '':
                continue # No need to depend on the definition when a declaration is sufficient

            graph[local_type.name].append(dependency_type)

    def walk_depth_first(name):
        if name in levels_by_name:
            return levels_by_name[name]
        children = graph.get(name, None)
        level = 0 if not children else (1 + max(walk_depth_first(lname) for lname in sorted(children)))
        levels_by_name[name] = level
        names_by_level.setdefault(level, default=[]).append(name)
        return level

    for name in graph:
        walk_depth_first(name)

    levels = takewhile(lambda x: x is not None, (names_by_level.get(i, None) for i in count()))

    for level in levels:
        for name in level:
            for local_type in local_types.values():
                if name == local_type.name:
                    yield local_type
                    break

idb_path = Path(get_idb_path())
project_name = idb_path.stem.lower().replace(' ', '')
export("""C:\dev\work\MagnetarCraft\MagnetarCraft\\""", f'{project_name}_exe', f'game::{project_name}')
