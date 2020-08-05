import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os

path_to_json = 'data/'
json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('.json')]

mean = []
for js in (json_files):
    with open(os.path.join(path_to_json, js)) as json_file:
        json_data = json.load(json_file)
        samples = json_data['cll_experiment']['sample']
        
        cpus = []
        num = []
        tcm = []

        for p in samples: 
            cpus.append(p[0])
            num.append(p[2])
            tcm.append(p[3])

        
        plt.figure(1)
        plt.plot(cpus,num, label = js)
        plt.xlabel("CPU")
        plt.ylabel("PAPI_REAL_USEC")
        plt.legend()
        
        
        mean.append(np.mean(num))
        

plt.figure(2)
plt.plot(json_files, mean, color='blue')
plt.xlabel("CPU")
plt.ylabel("Media PAPI_REAL_USEC")
plt.title("Media de PAPI_REAL_USEC para cada fichero")
plt.show()




