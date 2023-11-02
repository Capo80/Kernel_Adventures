from ast import FunctionDef
from sre_constants import CALL
from tarfile import SYMTYPE
from turtle import register_shape
from get_func import get_surronding_function_from_file
from get_ref import get_symbol_references
from versions import kversion_5, kversion_6
import json
import os
import re
import time
import pandas as pd

CALL_DEPTH = 3
regex = [   # ORDER MATTERS
    ("macro" , re.compile(r"#define\s*(\w+)\s*\(([^,]*).*")),
    ("syscall" , re.compile(r"SYSCALL_DEFINE\d*\(([^,]*).*")), 
    ("function" , re.compile(r"\s*(\w+)\s*\(([^,]*).*")),
    ("struct" , re.compile(r"\.*struct \w+ (\w+)")),
]

SYMBOL = "commit_creds"
# SYMBOL = "switch_task_namespaces"



rows_list = []
for ver in [kversion_5, kversion_6]:

    while True:

        symbol_list = [(SYMBOL, 0)]

        # iterate over symbols
        while len(symbol_list) != 0 and symbol_list[0][1] < CALL_DEPTH:
            
            # extract curr sym information
            current_symbol = symbol_list.pop(0)
            sym_str = current_symbol[0]
            sym_depth = current_symbol[1]

            print(f"Parsing version {ver} for {sym_str} (Deph: {sym_depth})...")
    
            # find refs
            refs = get_symbol_references(sym_str, str(ver))
            # time.sleep(0.5) # dont spam api

            for r in refs:
                path = r.get("path")
                line_nb = r.get("line")
                if path == None or line_nb == None:
                    print("Error!! missing path or line_nb")
                    continue
                
                # some subsystems are not interesting
                subsystem = os.path.dirname(path) 
                if  any([
                    subsystem.startswith("scripts"),
                    subsystem.startswith("tools"),
                    subsystem.startswith("samples"),
                ]):
                    continue

                # assembly not supported
                file = os.path.basename(path)
                if file.endswith(".S"):
                    continue
                
                # iterate over multiple ref in same file
                for l in line_nb.split(","):

                    # find function that we are in
                    caller = get_surronding_function_from_file(path, str(ver), int(l)-1)

                    if caller.startswith("EXPORT_SYMBOL"):
                        continue
                    
                    
                    print(caller.encode("utf-8"))
                    for name, reg in regex:
                        res = reg.search(caller)
                        if res != None:
                            caller = res.group(1)
                            type = name

                            if (name == "function" or name == "macro"):
                                symbol_list.append((caller, sym_depth+1))
                            break
                    if res == None:
                        type = "error"
                    
                    # add to information list
                    new_row = {}
                    new_row["Kernel Version"] = str(ver)
                    new_row["Subsystem"] = subsystem
                    new_row["File"] = file
                    new_row["Callee"] = sym_str
                    new_row["Caller"] = caller
                    new_row["Caller Type"] = type

                    print(json.dumps(new_row, indent=2))
                    rows_list.append(new_row)

                    # exit()

                
                
        if not ver.go_next():
            break


df = pd.DataFrame(rows_list)
df.to_csv("references_commit_creds_3_deep.csv")
# df.to_csv("references_namespace_3_deep.csv")