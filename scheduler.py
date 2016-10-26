import socket
import sys
from subprocess import Popen

ip = "127.0.0.1"
port = 8002

with open(str(sys.argv[2])) as f:
    content = f.readlines()
    for line in content:
        if line.split()[0] == str(sys.argv[1]):
            ip = line.split()[1]
            port = line.split()[2]

print(ip)
print(port)

port = int(port) + 1
buffersize = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((ip, port))
s.listen(1)

while 1:
    conn, addr = s.accept()
    data = conn.recv(buffersize)
    print("Start job "+data)
    Popen("python "+data,shell=True)
    conn.close()

