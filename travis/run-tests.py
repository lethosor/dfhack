import os, subprocess, sys

MAX_TRIES = 5

dfhack = 'Dwarf Fortress.exe' if sys.platform == 'win32' else './dfhack'
test_stage = 'test_stage.txt'

def get_test_stage():
    if os.path.isfile(test_stage):
        return open(test_stage).read().strip()
    return '0'

os.chdir(sys.argv[1])
if os.path.exists(test_stage):
    os.remove(test_stage)

tries = 0
while True:
    tries += 1
    stage = get_test_stage()
    print('Run #%i: stage=%s' % (tries, get_test_stage()))
    if stage == 'done':
        print('Done!')
        os.remove(test_stage)
        sys.exit(0)
    elif stage == 'fail':
        print('Failed!')
        os.remove(test_stage)
        sys.exit(1)
    if tries > MAX_TRIES:
        print('Too many tries - aborting')
        sys.exit(1)

    # os.system(dfhack)
    process = subprocess.Popen([dfhack], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    out, err = process.communicate()

    # print(repr(out))
    # print(repr(err))
    # tests_started = False
    # for line in process.stdout.readlines():
    #     if 'dfhack-test-start' in line:
    #         tests_started = True
    #     elif tests_started:
    #         sys.stdout.write(text)
    #         sys.stdout.flush()
    #     else:
    #         sys.stdout.write('bad? ' + repr(line) + '\n')
