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