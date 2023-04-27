from os import stat
from tracemalloc import start
from stats_kprobe import *
import matplotlib.pyplot as plt

new_info = {"other": 0, "jmp*": 0}
for func in starts:
    # print(func)
    if (func[0] == "j"):
        new_info["jmp*"] += 1
    elif (starts[func] > 50):
        new_info[func] = starts[func]
    else:
        new_info["other"] += 1

plt.bar(range(len(new_info)), list(new_info.values()), align='center')

plt.xticks(range(len(new_info)), list(new_info.keys()),rotation=45, ha='right')

plt.show()
