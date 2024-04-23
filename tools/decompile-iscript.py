# This is a rudimentary (and incomplete) tool for iscript decompilation

def decompile(in_path, out_path):
    result = ''

    with open(in_path, 'rb') as f:
        data = f.read()

    def p(i):
        nonlocal result
        result += hex(data[i])[2:].rjust(2, '0')

    def nl():
        nonlocal result
        result += '\n'

    def s():
        nonlocal result
        result += ' '

    i = 0
    while i < len(data):
        if i < 1372:
            p(i)
            i += 1
        elif i >= 0x97e6: # Do not hard-code this; take it from the first 2 bytes of the file
            p(i+1)
            p(i)
            i += 2
        elif data[i] == 0x53 and data[i + 1] == 0x43 and data[i + 2] == 0x50 and data[i + 3] == 0x45:
            p(i)
            p(i+1)
            p(i+2)
            p(i+3)
            nl()
            p(i+4)
            p(i+5)
            p(i+6)
            p(i+7)
            s()
            label_count = data[i + 4] + 1
            i += 8
            for j in range(label_count):
                #p(i)
                #p(i+1)
                result += 'XXXX'
                if j != label_count - 1:
                    s()
                i += 2
        elif data[i] == 0 and i + 2 < len(data) and data[i + 1] != 0x53:
            result += 'playfram '
            s()
            p(i+2)
            p(i+1)
            i += 3
        elif data[i] == 5:
            result += 'wait '
            p(i + 1)
            i += 2
        elif data[i] == 7:
            result += 'goto '
            s()
            result += 'XXXX'
            #p(i+2)
            #p(i+1)
            i += 3
        elif data[i] == 0x16:
            result += 'end'
            i += 1
        elif data[i] == 0x1A:
            result += 'playsndbtwn '
            p(i + 2)
            p(i + 1)
            s()
            p(i + 4)
            p(i + 3)
            i += 5
        elif data[i] == 0x2A:
            result += 'gotorepeatattk'
            i += 1
        elif data[i] == 0x2C:
            result += 'engest '
            p(i + 1)
            i += 2
        elif data[i] == 0x2E:
            result += 'nobrkcodestart'
            i += 1
        elif data[i] == 0x2F:
            result += 'nobrkcodeend'
            i += 1
        else:
            p(i)
            i += 1
        nl()

    with open(out_path, 'wt') as f:
        f.write(result)

# TODO: read these from the command line instead of hard-coding them

base_path = 'out/build/x86-Debug/Magnetar'
base_path = '.'

in1 = f'{base_path}/iscript.bin'
in2 = f'{base_path}/iscript2.bin'

out1 = f'{base_path}/iscript.hex'
out2 = f'{base_path}/iscript2.hex'

decompile(in1, out1)
decompile(in2, out2)
