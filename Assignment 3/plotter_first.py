import matplotlib as mlp
import matplotlib.pyplot as plt
import numpy as np
import sys

time = []
cwnd = []

cwndfile = sys.argv[1]+".cwnd"
imgfile = sys.argv[1]+".png"

with open(cwndfile) as file:
    data = file.read()
    lines = [line for line in data.split("\n") if line]
    for line in lines:
        line = line.split("\t")
        time.append(float(line[0]))
        cwnd.append(float(line[1]))
    
plt.title(sys.argv[1])
plt.xlabel("Time(s)")
plt.ylabel("Congestion Window Size")
plt.xticks(np.arange(0,31,2))
plt.yticks(np.linspace(10*(min(cwnd)//10),10*(max(cwnd)//10),num=10))
plt.plot(time, cwnd)
plot = plt.gcf()
plt.show()
plot.savefig(imgfile)
