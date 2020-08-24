import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os

path_to_json = 'data/'
json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('40.json')]

mean = []
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


tests = num_tests[0]
prueba = []

N = tests[0]


for i in number_cpus:
    number = []
    prueba = nums[0]
    nums.pop(0)
    for j in range(i):
        dummy = []
        for k in range(N):
            dummy.append(prueba[k +j*N])
           
        number.append(dummy)
    fig, ax = plt.subplots()
    plt.xlabel("CPU")
    plt.ylabel("PAPI_REAL_USEC")
    ax.set_title("BOXPLOT")
    ax.boxplot(number)

plt.show()
    





    


    




 




