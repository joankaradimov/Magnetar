import re

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
        if ref_func and ref_func.startEA == ref:
            result.append(ref)

    return result

def get_type(ref):
    ref_type = GetType(ref) or GuessType(ref)
    ref_name = GetFunctionName(ref)

    if ref_type == None:
        # TODO: there might be a bug in GuessType
        # Use the hex rays python bindings in IDA 6.6
        return None

    func_flags = GetFunctionAttr(ref, FUNCATTR_FLAGS)
    is_lib = func_flags & FUNC_LIB
    if is_lib:
        return None

    ref_type = ref_type.replace('__far', '')
    ref_type = ref_type.replace('this,', 'this_,')
    ref_type = ref_type.replace('this@', 'this_@')
    ref_type = ref_type.replace('this)', 'this_)')
    ref_type = ref_type.replace(' __noreturn ', ' ')

    if '__cdecl' in ref_type:
        ref_type = ref_type.replace('__cdecl(', '(__cdecl*%s)(' % ref_name, 1)
    elif '__usercall' in ref_type:
        ref_type = re.sub(r'@<[^>]*>', '', ref_type)
        ref_type = ref_type.replace('__usercall(', '%s(' % ref_name, 1)
        ref_type += ' { throw "not implemented"; }' # TODO: handle this
    elif '__userpurge' in ref_type:
        ref_type = re.sub(r'@<[^>]*>', '', ref_type)
        ref_type = ref_type.replace('__userpurge(', '%s(' % ref_name, 1)
        ref_type += ' { throw "not implemented"; }' # TODO: handle this
    elif '__stdcall' in ref_type:
        ref_name = re.sub(r'\@\d+', '', ref_name)
        ref_type = ref_type.replace('__stdcall(', '(__stdcall*%s)(' % ref_name, 1)
    elif '__thiscall' in ref_type:
        ref_type = ref_type.replace('__thiscall(', '(__thiscall*%s)(' % ref_name, 1)
    elif '__fastcall' in ref_type:
        ref_type = ref_type.replace('__fastcall(', '(__fastcall*%s)(' % ref_name, 1)
    else:
        ref_type = ref_type.replace('(', '(*%s)(' % ref_name, 1)

    return ref_type

def get_declaration(ref):
    ref_type = get_type(ref)
    if ref_type == None:
        return ''

    ref_name = GetFunctionName(ref)
    ref_name = re.sub(r'\@\d+', '', ref_name)
    if ref_name.find('@') >= 0:
        return ''

    has_body = re.search(r'\{.*\}', ref_type)
    if has_body:
        return ref_type + '\n'
    else:
        return 'DECL_FUNC({decl}, {name}, {address});\n'.format(decl = ref_type, name = ref_name, address = hex(ref))

def get_definition(ref):
    ref_type = get_type(ref)
    if ref_type == None:
        return ''

    has_body = re.search(r'\{.*\}', ref_type)
    if has_body:
        return re.sub(r'\{.*\}', '', ref_type) + ';\n'
    else:
        return 'extern ' + ref_type + ';\n'

TYPES_HEADER_TEMPLATE = """#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <stdio.h>
#include <Windows.h>
#include <ddraw.h>
#include <DSound.h>

// Some convenience macros to make partial accesses nicer
// first unsigned macros:
#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
#define LODWORD(x)  (*((_DWORD*)&(x)))  // low dword
#define HIBYTE(x)   (*((_BYTE*)&(x)+1))
#define HIWORD(x)   (*((_WORD*)&(x)+1))
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
#define SLOBYTE(x)   (*((int8*)&(x)))
#define SLOWORD(x)   (*((int16*)&(x)))
#define SLODWORD(x)  (*((int32*)&(x)))
#define SHIBYTE(x)   (*((int8*)&(x)+1))
#define SHIWORD(x)   (*((int16*)&(x)+1))
#define SHIDWORD(x)  (*((int32*)&(x)+1))
#define SBYTEn(x, n)   (*((int8*)&(x)+n))
#define SWORDn(x, n)   (*((int16*)&(x)+n))
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

typedef __int8 _BYTE;
typedef __int16 _WORD;
typedef __int32 _DWORD;
typedef __int64 _QWORD;
typedef __int8 _TBYTE;

{declarations}

{definitions}"""

HEADER_TEMPLATE = """#pragma once

#include "starcraft_exe_types.h"

{definitions}"""

CPP_TEMPLATE = """#include "starcraft_exe_types.h"

#define DECL_FUNC(decl, func, offset) decl = (decltype(func)) offset;

{declarations}
#undef DECL_FUNC
"""

def export():
    declaration_macros = []
    definitions = []
    export_functions(definitions, declaration_macros)
    export_data(definitions, declaration_macros)

    type_declarations = []
    type_definitions = []
    export_types(type_declarations, type_definitions)

    root_dir = """C:\dev\work\star-plus\star-plus\\"""
    with open(root_dir + 'starcraft_exe.cpp', 'wt') as cpp_file:
        cpp_file.write(CPP_TEMPLATE.format(declarations = ''.join(declaration_macros)))

    with open(root_dir + 'starcraft_exe.h', 'wt') as header_file:
        header_file.write(HEADER_TEMPLATE.format(definitions = ''.join(definitions)))

    with open(root_dir + 'starcraft_exe_types.h', 'wt') as types_header_file:
        content = TYPES_HEADER_TEMPLATE.format(definitions = ''.join(type_definitions), declarations = ''.join(type_declarations))
        types_header_file.write(content)


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
    ]

    for identifier in blacklisted_identifiers:
        if identifier in text:
            return True
    return False

def export_functions(definitions, declaration_macros):
    text_segment = idaapi.get_segm_by_name('.text')
    for function_ea in Functions(text_segment.startEA, text_segment.endEA):
        declaration = get_declaration(function_ea)
        definition = get_definition(function_ea)

        if is_blacklisted(declaration):
            continue

        declaration_macros.append(declaration)
        definitions.append(definition)

def export_data(definitions, declaration_macros):
    data_segment = idaapi.get_segm_by_name('.data')

    ea = data_segment.startEA
    while ea != idc.BADADDR:
        ea = NextHead(ea, data_segment.endEA)

        data_type = GetType(ea) or GuessType(ea)
        data_name = idaapi.get_name(ea, ea)

        if data_name == None:
            continue
        elif '[' in data_type: # array -> pointer
            data_type = re.sub('\[[^\]]*\]', '* ' + data_name, data_type, count = 1)
            declaration = '{data_type} = (decltype({data_name} + 0)) {address};\n'.format(data_type = data_type, data_name = data_name, address = hex(ea))
        elif '*)(' in data_type: # function pointer -> reference to function pointer
            data_type = data_type.replace('*)', '*&' + data_name + ')',  1)
            declaration = '{data_type} = *((decltype(&{data_name})) {address});\n'.format(data_type = data_type, data_name = data_name, address = hex(ea))
        else: # scalar value -> reference
            data_type = data_type + '& ' + data_name
            declaration = '{data_type} = * ((decltype(&{data_name})) {address});\n'.format(data_type = data_type, data_name = data_name, address = hex(ea))

        definition = 'extern ' + data_type + ';\n'

        if is_blacklisted(declaration):
            continue

        definitions.append(definition)
        declaration_macros.append(declaration)

def is_type_blacklisted(type_ordinal):
    local_type_name = GetLocalTypeName(type_ordinal)
    if local_type_name == None:
        return False # TODO: determine when this happens
    if local_type_name in {'RECT', 'POINT', '_GUID'}:
        return True
    if local_type_name.startswith('IDirectDraw'):
        return True
    return False

class Type(object):
    body_pattern = re.compile('{.*};')
    base_type_pattern = re.compile(r's*:\s*\w+\s*;\n')
    type_qualifier_pattern = re.compile(r'\b(const|unsigned|signed|struct|union|class|enum)\b')

    keyword_constructor = {}

    def __new__(cls, type_definition):
        type_keyword = type_definition.split(' ')[0]
        constructor = cls.keyword_constructor.get(type_keyword)
        if constructor:
            self = object.__new__(constructor)
            self.definition = type_definition.strip() + ';\n\n'
            return self
        else:
            raise Exception('key word not registered') # TODO: proper exception type

    @classmethod
    def register_keyword(cls, keyword, subclass):
        cls.keyword_constructor[keyword] = subclass

    @property
    def definition_without_body(self):
        if not hasattr(self, '_definition_without_body'):
            local_type_oneline = self.definition.replace('\n', '')
            local_type_no_body = self.body_pattern.sub(';\n', local_type_oneline)
            self._definition_without_body = self.base_type_pattern.sub(';\n', local_type_no_body)
        return self._definition_without_body

    @property
    def name(self):
        if not hasattr(self, '_name'):
            definition_without_body = self.base_type_pattern.sub('', self.definition_without_body)
            name = definition_without_body.replace(';', '').split()[-1]
            self._name = name
        return self._name

def keywords(*keywords):
    def keywords_inner(cls):
        for keyword in keywords:
            Type.register_keyword(keyword, cls)
    return keywords_inner

@keywords('enum')
class EnumType(Type):
    @property
    def declaration(self):
        return self.definition_without_body

    @property
    def dependencies(self):
        return set() # TODO

@keywords('struct', 'union', 'class')
class CompositionType(Type):
    fields_pattern = re.compile(r'{(.*)}', re.DOTALL)

    @property
    def fields(self):
        body_fields = self.fields_pattern.findall(self.definition)
        return body_fields[0].split(';') if len(body_fields) > 0 else []

    @classmethod
    def simplify_field(cls, field):
        field = cls.type_qualifier_pattern.sub('', field)
        field = re.sub(r'\s+', ' ', field)
        return field.strip()

    @property
    def declaration(self):
        return self.definition_without_body

    @property
    def dependencies(self):
        fields = map(self.simplify_field, self.fields)
        fields = filter(lambda field: '*' not in field, fields)
        field_types = { field.split(' ')[0] for field in fields if field != '' }
        return field_types

@keywords('typedef')
class TypedefType(Type):
    function_ptr_type_pattern = re.compile(r'^typedef (?P<return_type>.*) \((\w+\s*)*\*\s*(?P<type_name>\w+)\)\((?P<args>.*)\);')

    @property
    def declaration(self):
        return ''

    @property
    def dependencies(self):
        function_ptr_type_match = self.function_ptr_type_pattern.match(self.definition_without_body)
        if function_ptr_type_match:
            result = set()
            result.add(function_ptr_type_match.group('return_type'))
            for argument in function_ptr_type_match.group('args').split(','):
                argument = self.type_qualifier_pattern.sub('', argument)
                argument = argument.replace('*', '')
                argument_name = re.split('\s+', argument)[-1]
                result.add(argument_name)
            return result
        else:
            return set() # TODO

    @property
    def name(self):
        if not hasattr(self, '_name'):
            function_ptr_type_match = self.function_ptr_type_pattern.match(self.definition_without_body)
            if function_ptr_type_match:
                self._name = function_ptr_type_match.group('type_name')
            else:
                return super(self.__class__, self).name
        return self._name

def export_types(declarations, definitions):
    existing_type_names = set()
    local_types = []

    for type_ordinal in range(1, GetMaxLocalType()):
        if is_type_blacklisted(type_ordinal):
            continue
        
        type_name = GetLocalTypeName(type_ordinal)

        if type_name in existing_type_names:
            continue
        else:
            existing_type_names.add(type_name)

        try:
            local_type_definition = GetLocalType(type_ordinal, PRTYPE_MULTI | PRTYPE_TYPE)
            local_type = Type(local_type_definition)
            local_types.append(local_type)
        except:
            continue

        wat = local_type.definition_without_body # TODO: determine why nothing works without this line

    for local_type in sort_topologically(local_types):
        declarations.append(local_type.declaration)
        definitions.append(local_type.definition)


from collections import defaultdict
from itertools import takewhile, count

def sort_topologically(local_types):
    graph = {}
    levels_by_name = {}
    names_by_level = defaultdict(set)

    for local_type in local_types:
        graph[local_type.name] = local_type.dependencies

    def walk_depth_first(name):
        if name in levels_by_name:
            return levels_by_name[name]
        children = graph.get(name, None)
        level = 0 if not children else (1 + max(walk_depth_first(lname) for lname in children))
        levels_by_name[name] = level
        names_by_level[level].add(name)
        return level

    for name in graph:
        walk_depth_first(name)

    levels = takewhile(lambda x: x is not None, (names_by_level.get(i, None) for i in count()))

    sorted_local_types = []
    for level in levels:
        for name in level:
            for local_type in local_types:
                if name == local_type.name:
                    sorted_local_types.append(local_type)
                    break

    return sorted_local_types

export()
