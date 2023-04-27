from distutils.log import error
from os import stat
import gdb 
import json

SYMS_TOTAL = 139262
sym_file = open("kprobe_syms.txt", "r")

stats = {}
errors = []
non_nop_func = []
c = 0
for line in sym_file:
    # line_split = line.split(":\t\t")
    try:
        res = gdb.execute("x/i " + line.strip(), to_string=True)
        # print(res, line.strip())
        instr = res.split(":\t")[1].strip()
        if(instr[:3] == "nop"):
            non_nop_func.append(line.strip())
        if (stats.get(instr) == None):
            stats[instr] = 1
        else:
            stats[instr] += 1
    except:
        errors.append(line.strip())
    c += 1
    if (c % 1000 == 0):
        print("done (", c, "/203525)")
    

stats_file = open("stats_kprobe.py", "w")
stats_file.write("starts = " + json.dumps(stats) + "\n")
stats_file.write("errors = {\"" + "\",\"".join(errors) + "\"}\n")


func_file = open("funcs.py", "w")
func_file.write("funcs = {\"" + "\",\"".join(non_nop_func) + "\"}\n")

stats_file.close()
sym_file.close()
func_file.close()