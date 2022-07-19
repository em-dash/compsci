import subprocess

file = "test.py"

control = subprocess.run(["md5sum", file], capture_output=True)
if control.returncode != 0:
    print("md5sum failed")
    exit()

test = subprocess.run(["./hash", file], capture_output=True)
if test.returncode != 0: 
    print("test failed: non-zero exit code")
    exit()

if control.returncode == 0 and test.returncode == 0:
    if control.stdout == test.stdout:
        print("test success")
    else:
        print("test failed: incorrect output")
