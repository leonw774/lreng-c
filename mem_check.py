# filename = 'mem_check_out.txt'
filename = 'debug.txt'
mems = dict()
funcs = dict()
with open(filename, 'rb') as f:
    for linenum, line in enumerate(f):
        if not line.startswith(b'[mem_check]'):
            continue
        strip_line = line.rstrip()[11:]
        try:
            func, code_pos, addr, *info = strip_line.split(b' ')
        except Exception as e:
            print(strip_line)
            raise e
        if func == b'[free]':
            if addr in mems:
                del mems[addr]
            else:
                print('used free??')
                print(strip_line)
        elif func.endswith(b'alloc]'):
            if addr in mems:
                print('double alloc??')
                print(strip_line)
            else:
                mems[addr] = (linenum+1, func, code_pos, info)

print('\n'.join([
    f'{linenum}\n{func.decode()} {code_pos.decode()} {addr.decode()} {info}'
    for addr, (linenum, func, code_pos, info) in mems.items()
]))