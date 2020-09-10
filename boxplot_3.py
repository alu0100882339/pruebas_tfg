import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os

path_to_json = 'data/data_2/'
json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('40.json')]


sequential_sample = []
with open(os.path.join(path_to_json, 'sequential_2.json')) as sequential_file:
    json_data = json.load(sequential_file)
    sequential_sample.append(json_data['cll_experiment']['sample'])

sequential_nums = []

for p in sequential_sample:
    for j in p:
        sequential_nums.append(j[2])


sample = []
num_tests = []
for js in (json_files):
    with open(os.path.join(path_to_json, js)) as json_file:
        json_data = json.load(json_file)
        sample.append(json_data['cll_experiment']['sample'])
        num_tests.append(json_data['cll_experiment']['NUMTESTS'])
         
       
        number_cpus = []
    for p in sample:
        for i in p:
            number_cpus.append(i[1])


total_cpus = []
for i in range(32):
    total_cpus.append(i+1)


number_cpus = list(dict.fromkeys(number_cpus))    

cpus = []
nums = []
tcms = []

for i in number_cpus:
    cpu = []
    num = []
    tcm = []

    for p in sample:
        for j in p:
            if j[1] == i:
                cpu.append(j[0])
                num.append(j[2])
                tcm.append(j[3])

    cpu = list(dict.fromkeys(cpu))
    cpus.append(cpu)
    nums.append(num)
    tcms.append(tcm)


sequential = np.mean(sequential_nums)

print(sequential)


number_cpus.sort()
print(number_cpus)
N = len(number_cpus)
parallel = []
for i in range (N):
    
    parallel.append(np.mean(nums[i]))



parallel.sort(reverse=True)
print(parallel)

speedup = []

for i in range(len(parallel)):
    speedup.append(sequential / parallel[i])
    
   

print(speedup)

fig, ax = plt.subplots()
speedup_boxplot = []
for i in range (N):
    for j in nums[i]:
        dummy = []
        dummy.append(sequential / j)

    speedup_boxplot.append(dummy)

speedup_boxplot.sort()
print(speedup_boxplot)
ax.boxplot(speedup_boxplot, showfliers=True)
plt.plot(number_cpus, number_cpus, "r--")

plt.show()