
import socket
import threading
from ..configHandler import getESPCount

BROADCAST_IP = "255.255.255.255"
BROADCAST_PORT = 9999

ESP_LIST = []
EXPECTED_ESP = getESPCount()

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((BROADCAST_IP, BROADCAST_PORT))

setup = True
while setup:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    if "Hej hej :) Jag är" in data:
        data = data[-5:]
        ESP_LIST.append((int)(data, addr))

    if len(ESP_LIST) == EXPECTED_ESP:
        setup = False
        sock.close()

def connectToESP(ip: str, port: int):
    sock.bind(ip, port)
    id = port - 10000
    message = "OK"
    sock.sendto(message.encode('utf-8'), ip)
    
    loop = True
    while loop:
        data, addr = sock.recvfrom(1024)
        print("ESP",id,":   ", data)

for esp in ESP_LIST:
    thread = threading.Thread(target=setup, args=(esp[1], esp[0]), daemon=True)
    thread.start()