import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os


path_to_json = '../data/data_1/'
json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('40.json')]

sequential_sample = []
with open(os.path.join(path_to_json, 'sequential.json')) as sequential_file:
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

error_calc = [line.rstrip('\n') for line in open('data/salidas_calculo/error.txt')]
error_calc = np.array(error_calc, dtype=np.float32)

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
efficiency = []
for i in range(len(parallel)):
    speedup.append(sequential / parallel[i])
    efficiency.append(sequential / (number_cpus[i]*parallel[i]))
   

print(speedup)
print(efficiency)


plt.figure(1)
plt.plot(number_cpus,speedup, linestyle='-', marker='o')
plt.plot(number_cpus, number_cpus, "r--")


plt.title('Speedup')
plt.xlabel('Processors')

plt.figure(2)
plt.plot(number_cpus,efficiency, linestyle='-', marker='o')
plt.title('Efficiency')
plt.xlabel('Processors')

plt.figure(3)
plt.scatter(total_cpus,error_calc)
z = np.polyfit(total_cpus,error_calc,1)
p = np.poly1d(z)
plt.plot(total_cpus,p(total_cpus), "r--")


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