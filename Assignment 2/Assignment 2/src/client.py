import socket
from _thread import *
import threading


user = ''
ERROR_100 = 'ERROR 100 Malformed username\n\n'
ERROR_101 = 'ERROR 101 No user registered\n\n'
ERROR_102 = 'ERROR 102 Unable to send\n\n'
ERROR_103 = 'ERROR 103 Header incomplete\n\n'

def send_fun(s): # thread for sending
    
    while True:
        data = ''
        space = 0
        while True:
            print(':',end="")
            data = input()
            if(len(data)==0):
                continue
            space = data.find(' ')
            if(data[0] != '@' or space == -1):
                print("Invalid message. Please type again") # Invalid format
            else:
                break
        msg = "SEND " + data[1:space] + "\nContent-length: " + str(len(data)-space-1) + "\n\n" + data[space+1:] # send message to server

        try:
            s.sendall(msg.encode('ascii'))
        except:
            return
        resp = s.recv(1024).decode() # wait for server's response
        
        if(resp == 'SENT ' + data[1:space] + "\n\n"):
            print("Delivered")
        elif(resp == ERROR_101):
            print("TCP Connection for Recieve not established")
        elif(resp == ERROR_102):
            print("User not found")
        else:
            print("Header incomplete")


def recv_fun(s):

    while True:
        try:
            data = s.recv(1024)
        except:
            return
        data = data.decode()
        
        space=data.find(' ')
        
        if(space==-1 or data[0:space]!='FORWARD'):
            s.sendall(ERROR_103.encode('ascii'))
        else:
            data=data[space+1:]
            n = data.find('\n')
            if(n==-1):
                s.sendall(ERROR_103.encode('ascii'))
            else:
                sender = data[:n]
                data = data[n+1:]
                space = data.find(' ')
                if(space==-1 or data[:space]!='Content-length:'):
                    s.sendall(ERROR_103.encode('ascii'))
                else:
                    data=data[space+1:]
                    n=data.find('\n')
                    if(n==-1 or len(data)<n+2 or data[n+1]!='\n'):
                        s.sendall(ERROR_103.encode('ascii'))
                    else:
                        lent=data[:n]
                        if(not lent.isnumeric()):
                            s.sendall(ERROR_103.encode('ascii'))
                        else:
                            lent = int(lent)
                            msg = data[n+2:]
                            if(len(msg)!=lent): # length specified in header is not correct
                                s.sendall(ERROR_103.encode('ascii'))
                            else:
                                resp = "RECEIVED " + sender + "\n\n"
                                s.sendall(resp.encode('ascii'))
                                print("@" + sender + ": "+msg)

# Take input
print("Enter HOST")
HOST = input()
print("Enter PORT")
PORT = int(input())

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sendsock: ##= new socket to send messages
    sendsock.connect((HOST, PORT)) # establish TCP connection
    print("Enter username")
    user = input()
    
    while True:
        data = "REGISTER TOSEND " + user + "\n\n"
        sendsock.sendall(data.encode('ascii'))
        print("Request REGISTER TOSEND sent")
        data = sendsock.recv(1024)
        data = data.decode()
        if(data == "REGISTERED TOSEND " + user + "\n\n"):
            print("Registered to send")
            break
        if(data == ERROR_100):
            print("Invalid username")
        else:
            print("Registration not complete")
        user = input()
    
    start_new_thread(send_fun, (sendsock,)) # start new thread to send messages

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as recvsock: # new socket to recieve messages
        recvsock.connect((HOST,PORT)) # establish TCO connection
        
        while True:
            data = "REGISTER TORECV " + user + "\n\n"
            recvsock.sendall(data.encode('ascii'))
            print("Request REGISTER TORECV sent")
            data = recvsock.recv(1024)
            data = data.decode()
            # print(data)
            if(data == "REGISTERED TORECV " + user + "\n\n"):
                print("Registered to recieve")
                break
            if(data == ERROR_100):
                print("Invalid username")
            else:
                print("Registration not complete")
        
        recv_fun(recvsock) # start new thread to recieve messages
        sendsock.close()