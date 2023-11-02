
import re
import requests
import json
from versions import kversion_5, kversion_6
from pycparser import c_ast, parse_file
from git import Repo

def setup_new_kernel_version(version):

    repo = Repo("linux")
    if repo.git.describe(["--tags", "--abbrev=0"]) != version:
        repo.git.checkout("--force", version)

def get_surronding_function_from_file(file_path, version, line_nb):

    setup_new_kernel_version(version)

    full_path = "linux/" + file_path

    with open(full_path, "r") as fd:
        lines = fd.readlines()

        #check if we are inside macro
        if (lines[line_nb].startswith("#define")):
            return lines[line_nb]

        #print(lines[line_nb])
        c = line_nb
        while  any([
            lines[c].startswith(" \t"),
            lines[c].startswith(" "),
            lines[c].startswith("\t"),
            lines[c].strip() == "",
            lines[c].startswith("{"),
            lines[c].startswith("\""),
            lines[c].startswith("#"),
            lines[c].startswith("/*"),
            lines[c].startswith("//"),
            lines[c].strip().endswith(":"),
        ]):
            # print(lines[c].encode("utf-8"))
            c -= 1
        #print(lines[c].encode("utf-8"))
        
        return lines[c]

if __name__ == "__main__":
    # file = "security/smack/smackfs.c"
    # version = str(kversion_6)
    # print(get_surronding_function_from_file(file, "latest", 2787-1))

    print(setup_new_kernel_version("v6.2"))
    
    