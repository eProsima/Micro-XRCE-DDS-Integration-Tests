import argparse
import sys
import subprocess
import os
import shutil

def call(command, wd=os.getcwd()):
    try:
        p = subprocess.Popen(command, shell=True, cwd=wd)
        returncode = p.wait() 
        if returncode != 0:
            print(command, " terminated with ", returncode, file=sys.stderr)
        else:
            print(command, " returned ", returncode, file=sys.stderr)
    except OSError as e:
        print("Execution failed:", e, file=sys.stderr)
    return returncode

def prepare(args):
    return call("git clone " + args.url + " " + args.dest + " --branch " + args.branch)

def build(args):
    build_dir = os.path.join(os.getcwd(), args.dest, "build")

    if (os.path.exists(build_dir)):
        shutil.rmtree(build_dir)
    os.mkdir(build_dir)

    return call("cmake .. -DEPROSIMA_BUILD=ON", build_dir), build_dir

def install(args, build_output):
    return call("make install", build_output)


def main():
    parser = argparse.ArgumentParser()
    # Do some argument parsing
    parser.add_argument("url")
    parser.add_argument("branch")
    parser.add_argument("dest")
    args = parser.parse_args()
    
    if (not prepare(args)):
        result, build_dir = build(args)
        if (not result and os.path.exists(build_dir)):
            return install(args, build_dir)
        else:
            return -1
    else:
        return -1

if __name__ == "__main__":
    # execute only if run as a script
    main()