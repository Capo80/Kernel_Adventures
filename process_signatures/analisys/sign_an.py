import matplotlib.pyplot as plt
from matplotlib import pyplot, transforms

def read_file(filename):

    info = {}

    with open(filename) as fd:
        for line in fd:
            splitted = line.strip().split(", ")
            timestamp = int(splitted[2])
            hash = splitted[1]
            name = splitted[0]
            if info.get(timestamp) == None:
                info[timestamp] = [(name, hash)]
            else:
                info[timestamp].append((name, hash)) 
    
    return info

def most_common(filename):

    info = read_file(filename)

    common = {}
    for timestamp in info:
        for item in info[timestamp]:
            if common.get(item[0]) == None:
                common[item[0]] = 1
            else:
                common[item[0]] += 1

    return common

def hash_gather(filename):

    info = read_file(filename)

    hash_info = {}
    for timestamp in info:
        for item in info[timestamp]:
            if hash_info.get(item[0]) == None:
                hash_info[item[0]] = [item[1]]
            else:
                if item[1] not in hash_info[item[0]]:
                    hash_info[item[0]].append(item[1])

    return hash_info

def hash_count(filename):

    info = hash_gather(filename)

    hash_count = {}
    for name in info:
        hash_count[name] = len(info[name])

    return hash_count


def miss_count(filename):

    info = hash_gather(filename)

    hash_count = {}
    for name in info:
        if len(info[name]) != 1:
            hash_count[name] = len(info[name])

    return hash_count

#print(most_common("info.txt"))
#print(read_file("info.txt"))

base = pyplot.gca().transData
rot = transforms.Affine2D().rotate_deg(90)

info = most_common("info.txt")
# info = miss_count("info.txt")

plt.barh(range(len(info)), list(info.values()), align='center')
plt.yticks(range(len(info)), list(info.keys()))

plt.show()
