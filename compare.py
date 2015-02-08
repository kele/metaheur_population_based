my = []

with open("my.txt") as x:
    for line in x:
        my.append(int(line))

opt = []
with open("wtopt40.txt") as x:
    for line in x:
        opt.append(int(line))

for i in range(len(my)):
    if opt[i] != 0:
        print(my[i]/opt[i])
    else:
        print('abs', my[i] - opt[i])
