import matplotlib as mlp
import matplotlib.pyplot as plt
import numpy as np
import sys


configuration = sys.argv[1]

cwndfile1 = configuration + "_1" + ".cwnd"
imgfile1 = 'config'+configuration + "_1" + ".png"

cwndfile2 = configuration + "_2" + ".cwnd"
imgfile2 = 'config'+configuration + "_2" + ".png"

cwndfile3 = configuration + "_3" + ".cwnd"
imgfile3 = 'config'+configuration + "_3" + ".png"

with open(cwndfile1) as file:
    time = []
    cwnd = []
    data = file.read()
    lines = [line for line in data.split("\n") if line]
    for line in lines:
        line = line.split("\t")
        time.append(float(line[0]))
        cwnd.append(float(line[1]))
    plt.title(imgfile1)
    plt.xlabel("Time(s)")
    plt.ylabel("Congestion Window Size")
    plt.xticks(np.arange(0,31,2))
    plt.yticks(np.linspace(10*(min(cwnd)//10),10*(max(cwnd)//10),num=10))
    plt.plot(time, cwnd)
    plot = plt.gcf()
    plt.show()
    plot.savefig(imgfile1)

with open(cwndfile2) as file:
    time = []
    cwnd = []
    data = file.read()
    lines = [line for line in data.split("\n") if line]
    for line in lines:
        line = line.split("\t")
        time.append(float(line[0]))
        cwnd.append(float(line[1]))
    plt.clf()
    plt.title(imgfile2)
    plt.xlabel("Time(s)")
    plt.ylabel("Congestion Window Size")
    plt.xticks(np.arange(0,31,2))
    plt.yticks(np.linspace(10*(min(cwnd)//10),10*(max(cwnd)//10),num=10))
    plt.plot(time, cwnd)
    plot = plt.gcf()
    plt.show()
    plot.savefig(imgfile2)

with open(cwndfile3) as file:
    time = []
    cwnd = []
    data = file.read()
    lines = [line for line in data.split("\n") if line]
    for line in lines:
        line = line.split("\t")
        time.append(float(line[0]))
        cwnd.append(float(line[1]))
    plt.clf()
    plt.title(imgfile3)
    plt.xlabel("Time(s)")
    plt.ylabel("Congestion Window Size")
    plt.xticks(np.arange(0,31,2))
    plt.yticks(np.linspace(10*(min(cwnd)//10),10*(max(cwnd)//10),num=10))
    plt.plot(time, cwnd)
    plot = plt.gcf()
    plt.show()
    plot.savefig(imgfile3)