import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os


path_to_json_1 = '../data/data_1/'
path_to_json_2 = '../data/data_2/'

json_files_1 = [pos_json for pos_json in os.listdir(path_to_json_1) if pos_json.endswith('40.json')]
json_files_2 = [pos_json for pos_json in os.listdir(path_to_json_2) if pos_json.endswith('40.json')]


sequential_sample_1 = []
sequential_sample_2 = []


with open(os.path.join(path_to_json_1, 'sequential.json')) as sequential_file_1:
    json_data = json.load(sequential_file_1)
    sequential_sample_1.append(json_data['cll_experiment']['sample'])

with open(os.path.join(path_to_json_2, 'sequential_2.json')) as sequential_file_2:
    json_data = json.load(sequential_file_2)
    sequential_sample_2.append(json_data['cll_experiment']['sample'])


sequential_nums_1 = []
sequential_nums_2 = []

for p in sequential_sample_1:
    for j in p:
        sequential_nums_1.append(j[2])

for p in sequential_sample_2:
    for j in p:
        sequential_nums_2.append(j[2])




sample_1 = []
num_tests_1 = []

sample_2 = []
num_tests_2 = []


for js in (json_files_1):
    with open(os.path.join(path_to_json_1, js)) as json_file:
        json_data = json.load(json_file)
        sample_1.append(json_data['cll_experiment']['sample'])
        num_tests_1.append(json_data['cll_experiment']['NUMTESTS'])
         
       
        number_cpus_1 = []
    for p in sample_1:
        for i in p:
            number_cpus_1.append(i[1])


for js in (json_files_2):
    with open(os.path.join(path_to_json_2, js)) as json_file:
        json_data = json.load(json_file)
        sample_2.append(json_data['cll_experiment']['sample'])
        num_tests_2.append(json_data['cll_experiment']['NUMTESTS'])
         
       
        number_cpus_2 = []
    for p in sample_2:
        for i in p:
            number_cpus_2.append(i[1])

#error_calc = [line.rstrip('\n') for line in open('data/salidas_calculo/error.txt')]
#error_calc = np.array(error_calc, dtype=np.float32)

total_cpus = []
for i in range(32):
    total_cpus.append(i+1)




number_cpus_1 = list(dict.fromkeys(number_cpus_1))  
number_cpus_2 = list(dict.fromkeys(number_cpus_2))   

cpus_1 = []
nums_1 = []
tcms_1 = []

cpus_2 = []
nums_2 = []
tcms_2 = []

for i in number_cpus_1:
    cpu_1 = []
    num_1 = []
    tcm_1 = []

    for p in sample_1:
        for j in p:
            if j[1] == i:
                cpu_1.append(j[0])
                num_1.append(j[2])
                tcm_1.append(j[3])

    cpu_1 = list(dict.fromkeys(cpu_1))
    cpus_1.append(cpu_1)
    nums_1.append(num_1)
    tcms_1.append(tcm_1)


sequential_1 = np.mean(sequential_nums_1)

for i in number_cpus_2:
    cpu_2 = []
    num_2 = []
    tcm_2 = []

    for p in sample_2:
        for j in p:
            if j[1] == i:
                cpu_2.append(j[0])
                num_2.append(j[2])
                tcm_2.append(j[3])

    cpu_2 = list(dict.fromkeys(cpu_2))
    cpus_2.append(cpu_2)
    nums_2.append(num_2)
    tcms_2.append(tcm_2)


sequential_2 = np.mean(sequential_nums_2)



number_cpus_1.sort()

N = len(number_cpus_1)
parallel_1 = []
for i in range (N):
    
    parallel_1.append(np.mean(nums_1[i]))

number_cpus_2.sort()

N = len(number_cpus_2)
parallel_2 = []
for i in range (N):
    
    parallel_2.append(np.mean(nums_2[i]))


parallel_1.sort(reverse=True)
parallel_2.sort(reverse=True)


speedup_1 = []
efficiency_1 = []

speedup_2 = []
efficiency_2 = []

for i in range(len(parallel_1)):
    speedup_1.append(sequential_1 / parallel_1[i])
    efficiency_1.append(sequential_1 / (number_cpus_1[i]*parallel_1[i]))
   
for i in range(len(parallel_2)):
    speedup_2.append(sequential_2 / parallel_2[i])
    efficiency_2.append(sequential_2 / (number_cpus_2[i]*parallel_2[i]))
   

num_ef = []
for i in number_cpus_1:
    num_ef.append(1)

plt.figure(1)
al_1, = plt.plot(number_cpus_1,speedup_1, linestyle='-', marker='o', label='Algoritmo 1')
al_2, = plt.plot(number_cpus_2,speedup_2, linestyle='-', marker='o', label='Algoritmo 2')
op, = plt.plot(number_cpus_1, number_cpus_1, "r--", label='Valor ideal')

plt.title('Speedup')
plt.xlabel('Processors')



#plt.plot(number_cpus_2, number_cpus_2, "r--")



plt.legend(handles=[al_1,al_2,op])

plt.figure(2)
al_1, = plt.plot(number_cpus_1,efficiency_1, linestyle='-', marker='o', label='Algoritmo 1')
al_2, = plt.plot(number_cpus_2,efficiency_2, linestyle='-', marker='o', label='Algoritmo 2')
op, plt.plot(number_cpus_1,num_ef, "r--", label='Valor ideal')

plt.title('Efficiency')
plt.xlabel('Processors')



plt.legend(handles=[al_1,al_2,op])

plt.show()