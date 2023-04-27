from stats import *
import matplotlib.pyplot as plt

new_info = {"other": 0}
for func in starts:
    if (starts[func] > 50):
        new_info[func] = starts[func]
    else:
        new_info["other"] += 1

plt.bar(range(len(new_info)), list(new_info.values()), align='center')

plt.xticks(range(len(new_info)), list(new_info.keys()),rotation=45, ha='right')

plt.show()
