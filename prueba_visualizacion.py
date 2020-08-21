import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os

#path_to_json = 'data/'
#json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('.json')]

mean = []
with open('data/full_data.json') as json_file:
    json_data = json.load(json_file)
    samples = json_data['cll_experiment']['sample']
    num_tests = json_data['cll_experiment']['NUMTESTS']
    cpus = []
    num = []
    tcm = []

    for p in samples: 
        cpus.append(p[0])
        num.append(p[2])
        tcm.append(p[3])


cpus = list(dict.fromkeys(cpus))
num_tests = list(dict.fromkeys(num_tests))
N = num_tests[0]
M = len(cpus)
print(M)
print(N)
number = []
for i in range(M):
    dummy = []
    for j in range(N):
       dummy.append(num[j + i*N])
    
    number.append(dummy)
    





##for i in range(M):
##    print(number[i])
##    fig, ax = plt.subplots()
##    ax.set_title(i)
##    ax.boxplot(number[i])

fig, ax = plt.subplots()
plt.xlabel("CPU")
plt.ylabel("PAPI_REAL_USEC")
ax.set_title("BOXPLOT")
ax.boxplot(number)

print(number)
plt.show()

 




