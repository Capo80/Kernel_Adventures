from distutils.log import error
from os import stat
import gdb 
import json
#
sym_file = open("/proc/kallsyms", "r")

stats = {}
errors = []
c = 0
for line in sym_file:
    line_split = line.split(":\t\t")
    if line_split[1] == "T" or line_split[1] == "t":
        try:
            res = gdb.execute("x/i " + line_split[1], to_string=True)
            instr = res.split(":\t")[1].strip()
            print(res, line_split[1], instr)
            if (stats.get(instr) == None):
                stats[instr] = 1
            else:
                stats[instr] += 1
            break
        except:
            errors.append(line_split[1].strip())
    c += 1
    if (c % 1000 == 0):
        print("done (", c, "/203525)")

stats_file = open("stats.py", "w")
stats_file.write("starts = " + json.dumps(stats) + "\n")
stats_file.write("errors = {\"" + "\",\"".join(errors) + "\"}\n")

stats_file.close()
sym_file.close()