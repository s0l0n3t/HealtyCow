import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host ="192.168.1.103"
port =8000
s.connect((host,port))


s.send('1 0 25.23 100'.encode()) 
data = ''
data = s.recv(1024).decode()
print (data)


s.close ()

