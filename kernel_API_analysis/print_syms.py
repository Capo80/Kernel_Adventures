sym_file = open("all_syms.txt", "r")

syms = []
c = 0
for line in sym_file:
    line_split = line.split(":\t\t")
    syms.append(line_split[0].strip())


print(syms)

print(len(syms))
stats_file = open("syms.h", "w")
stats_file.write("syms = {\"" + "\",\n\"".join(syms) + "\"}\n")
stats_file.close()