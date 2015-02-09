import subprocess
from multiprocessing import Pool

opt = []
with open("wtopt40.txt") as x:
    for line in x:
        opt.append(int(line) + 1)

age_thresholds = [0.15, 0.40]
alpha_thresholds = [0.3, 0.6, 0.7, 0.8]
populs = [40, 100, 200, 400]
reiters = [1, 5, 10]

args = []
for age in age_thresholds:
    for alpha in alpha_thresholds:
        for p in populs:
            for r in reiters:
                args.append((age, alpha, p, r))


def run_test(args):
    age, alpha, population, reiters = args
    command = [
        'make', 'run40',
        'AGE_THRESHOLD=' + str(age),
        'ALPHA_THRESHOLD=' + str(alpha),
        'REITER_THRESHOLD=' + str(reiters),
        'POPULATION_SIZE=' + str(population)]

    my = subprocess.check_output(command).split()
    my = [int(y) for y in my]

    ratios = []
    bad_ratios = 0
    for i in range(125):
        r = my[i]/opt[i]
        if r > 10:
            bad_ratios += 1
        else:
            ratios.append(r)

    ratio_avg = 0
    for r in ratios:
        ratio_avg += r
    ratio_avg /= len(ratios)

    print('%f | Age: %f | Alpha: %f | Reiters: %d | Population: %d | Ratio_avg: %f | Bad ratios: %d' % (ratio_avg, age, alpha, reiters, population, ratio_avg, bad_ratios))


p = Pool(8)
p.map(run_test, args)
