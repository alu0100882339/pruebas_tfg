import matplotlib.pyplot as plt
import numpy as np
import pandas as pd 
import json, os

plt.figure(1)
plt.plot([1,2,3,4], [5,10,15,20], linestyle='-', marker='o')
plt.title('Ejemplo Gráfico Lineal')
plt.xlabel("Procesadores")
plt.ylabel("Tiempo de ejecución")

np.random.seed(10) 
data = np.random.normal(100, 20, 200) 
plt.figure(2)
plt.boxplot(data)
plt.title('Ejemplo Diagrama de cajas (Boxplot)')
plt.xlabel("Procesadores")
plt.ylabel("Tiempo de ejecución")

plt.show()