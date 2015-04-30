import re, os, sys
headers = [
    'pythonrun.h',
    'pyarena.h',
    'pyport.h',
    'pystate.h',
    'object.h',
]
def fatal(err):
    sys.stderr.write('%s\n' % err)
    sys.exit(1)
if len(sys.argv) <= 1:
    fatal('Missing argument 1 (directory)')
os.chdir(sys.argv[1])
print('// Generated - do not edit\n// %r' % headers)
types = []
funcs = []
for h in headers:
    with open(h) as f:
        contents = f.read()
    types.extend(re.findall(r'typedef\s*struct\s*\w+\s*\{[^}]+\}\s*([^;]+);', contents, re.MULTILINE))
    types.extend(re.findall(r'typedef\s*(?:struct|enum)\s*\{[^}]+\}\s*([^;]+);', contents, re.MULTILINE))
    types.extend(re.findall(r'typedef\s*struct\s*\w+[\s\*]*(\w+);', contents, re.MULTILINE))
    types.extend(re.findall(r'typedef\s*\w+\s*(\w+);', contents, re.MULTILINE))
    types.extend(re.findall(r'typedef\s*\w+\s*\(\s*\*\s*(\w+)\)\s*\([^)]+\);', contents, re.MULTILINE))
    funcs.extend(re.findall(r'PyAPI_FUNC\(([^)]+)\)\s*([^(]+)\s*\(([^)]+)\);', contents, re.MULTILINE))

for t in types:
    print('using CPython::%s;' % t)

func_names = []
for f in funcs:
    # Prevent duplicates
    if f[1] not in func_names:
        func_names.append(f[1])
        print('DFH_PY_FUNC(%s, %s, (%s));' % f)
