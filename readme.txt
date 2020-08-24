for js in (json_files):
    with open(os.path.join(path_to_json, js)) as json_file:
        json_data = json.load(json_file)
        samples = json_data['cll_experiment']['sample']




           print(len(num[0]))

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

plt.figure(3)
plt.boxplot(num)
#plt.show()




with open('data/full_data_8_40.json') as json_file:
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


#plt.show()



dummy = []
            for k in range(N):
                print(k)
                dummy.append(prueba[k + j*N])
    
    print(dummy)