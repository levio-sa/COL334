import os
import matplotlib.pyplot as plt
import numpy as np

print("Enter domain name / IP Address of destination")
dest = input()
iprtt = [] # ip vs RTT

print("Enter max hops(or blank to use default)")
maxHops = input()

if(maxHops==''):
    maxHops=30
else:
    maxHops = int(maxHops)

hop=1

isComplete = False

while(hop<=maxHops):
    shellstream = os.popen('ping -i ' + str(hop) + ' ' + dest)
    out = shellstream.read()
    x = out.find('Reply from')
    if(x==-1):
        iprtt.append(['-1',-1])
        print('hop ' + str(hop) + ' *')
    else:
        y = out.find(':', x)
        ip = out[x+11:y]
        print('hop '+str(hop)+ ' ' +  ip)
        if(out.find('Average')!=-1):
            isComplete = True
        shellstream = os.popen('ping ' + ip)
        out = shellstream.read()
        x = out.find('Average')
        y = out.rfind('ms')
        rtt = float(out[x+10:y])
        iprtt.append([ip,rtt])
        if(isComplete):
            break
    hop+=1

if(isComplete):
    fig, ax = plt.subplots()
    a=[]
    b=[]
    for i in range(len(iprtt)):
        if(iprtt[i][1]!=-1):
            a.append(i+1)
            b.append(iprtt[i][1])
    ax.plot(a,b,marker='.')
    ax.set_xlabel('Hop Number')
    ax.set_ylabel('Round Trip Time(ms)')
    ax.set_title('RTT vs Hops (' + dest + ')')
    plt.xticks(np.arange(min(a), max(a)+1, 1))
    plt.savefig(dest + '.png')

    print("traceroute for " + dest + " successful!")
    print("plot saved at "+ dest + '.png')
else:
    print("destination not reached. Try increasing max hops.")
        





    
