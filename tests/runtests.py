
import os, sys
import subprocess
import filecmp

sys.stderr.write("all tests\n");
test_list = [
    "define",
    "define1",
    "define2",
    "arity",
    "iftest",
    "import"]

# the build directory
#print(os.getcwd())

for s in test_list:
    print("running test", s)
    result = subprocess.run(['macros', '-i', s, '-o', 'junk1.txt', '-v0'], capture_output=True, text=True)
    if(result.returncode != 0)
        print(result.stderr);

