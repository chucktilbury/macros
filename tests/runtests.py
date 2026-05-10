
import os, sys
import subprocess
import filecmp

sys.stderr.write("all tests\n");
passing_test_list = [
    "define",
    "import",
    "define1",
    "define2",
    "arity",
    "ifelse"]

pdir = '../tests/passing/'
tdir = '../tests/temp'

# the build directory
#print(os.getcwd())

passed = 0
failed = 0

for s in passing_test_list:
    print("running test", s)
    result = subprocess.run(['macros', '-i', pdir+s, '-o', tdir+'junk1.txt', '-v0'], capture_output=True, text=True)
    if(result.returncode != 0):
        print("    fail: ", result.stderr)
        failed += 1
    else:
        print("    pass\n")
        passed += 1

print("tests run: ", passed + failed, " passed: ", passed, " failed: ", failed, "\n")

