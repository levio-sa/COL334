from _thread import *
import threading
import socket

ERROR_100 = b'ERROR 100 Malformed username\n\n'
ERROR_101 = b'ERROR 101 No user registered\n\n'
ERROR_102 = b'ERROR 102 Unable to send\n\n'
ERROR_103 = b'ERROR 103 Header incomplete\n\n'

HOST = '127.0.0.1'
PORT = 9999

send_table ={}
user_table ={} # user vs socket to recieve messages


# Function to handle created thread
def thread_fun(newsock):
    user='' # username
    while True:
        data = newsock.recv(1024)
        data = data.decode()
        if(len(data)>18 and data[-1]=='\n' and data[-2]=='\n'): # Checking validity of register request
            user = data[16:-2]
            if(user.isalnum()): # alphanumeric
                if(data[:16]=='REGISTER TOSEND '): # Socket to send message
                    data = 'REGISTERED TOSEND ' + user + '\n\n'
                    newsock.sendall(data.encode('ascii')) # positive response to client
                    break
                elif(data[:16]=='REGISTER TORECV '): # Socket to recieve message
                    user_table[user] = newsock # recieving tcp socket
                    data = 'REGISTERED TORECV ' + user + '\n\n'
                    newsock.sendall(data.encode('ascii')) # positive response to client
                    return # close thread after register request
                else: # Communication before registration
                    newsock.sendall(ERROR_101) 
            else: # Username malformed
                newsock.sendall(ERROR_100)
        else: # Communication before registration
            newsock.sendall(ERROR_101)
    
    while True: # sending socket listening
        data = newsock.recv(1024)
        data = data.decode()
        
        if(user_table.get(user) == None): # Recieving socket not present
            newsock.sendall(ERROR_101) # Communication before registration
            continue

        if(len(data)<5 or data[:5] != "SEND "):
            newsock.sendall(ERROR_103)
            continue
        
        data = data[5:]
        temp = data.find('\n')
        if(temp==-1):
            newsock.sendall(ERROR_103)
            continue
        
        rcpt = data[:temp]
        data = data[temp+1:]
        space = data.find(' ')
        if(space == -1 or data[:space]!='Content-length:'):
            newsock.sendall(ERROR_103)
            newsock.close() # close socket
            user_table[user].close() # close recieveing socket
            user_table.pop(user) # remove from hashtable
            return # close thread
        
        data=data[space+1:]
        n=data.find('\n')
        if(n==-1 or len(data)<n+2 or data[n+1]!='\n'):
            newsock.sendall(ERROR_103)
            newsock.close() # close socket
            user_table[user].close() # close recieveing socket
            user_table.pop(user) # remove from hashtable
            return # close thread
        
        lent=data[:n]
        if(not lent.isnumeric()):
            newsock.sendall(ERROR_103)
            newsock.close() # close socket
            user_table[user].close() # close recieveing socket
            user_table.pop(user) # remove from hashtable
            return # close thread
        
        lent = int(lent)
        msg = data[n+2:]
        if(len(msg)!=lent):
            newsock.sendall(ERROR_103)
            newsock.close() # close socket
            user_table[user].close() # close recieveing socket
            user_table.pop(user) # remove from hashtable
            return # close thread
        if(rcpt == user):
             # sending message to self
            newsock.sendall(ERROR_102) # receipient not found
            continue
        if(rcpt!='ALL' and user_table.get(rcpt)==None):
            newsock.sendall(ERROR_102) # receipient not found
            continue

        if(rcpt!='ALL'):
            msg = "FORWARD " + user + "\nContent-length: " + str(lent) + "\n\n" + msg
            user_table[rcpt].sendall(msg.encode('ascii'))
            data = user_table[rcpt].recv(1024) # wait for response
            if(data == ERROR_103):
                newsock.sendall(ERROR_103)
            else:
                data = "SENT " + rcpt + "\n\n"
                newsock.sendall(data.encode())
        else:
            x = 0
            for rcp in user_table:
                if(rcp!=user):
                    x = 1
                    msg1 = "FORWARD " + user + "\nContent-length: " + str(lent) + "\n\n" + msg
                    user_table[rcp].sendall(msg1.encode('ascii'))  
                    data = user_table[rcp].recv(1024) # wait for response
                    if(data == ERROR_103):
                        newsock.sendall(ERROR_103)
                        continue
            if(not x):
                newsock.sendall(ERROR_102) # NO RECIEPIENT
            data = "SENT ALL" + "\n\n"
            newsock.sendall(data.encode()) 





with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    
    sock.bind((HOST, PORT))
    print("Socket Bound to Host " + HOST + " and PORT " + str(PORT))
    
    sock.listen()
    print("Listening...")
    
    while True:
        
        newsock, addr = sock.accept()
        print("TCP Connection Request Accepted")

        print("Starting new thread")    
        start_new_thread(thread_fun, (newsock,))
        

        




