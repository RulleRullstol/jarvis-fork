import socket
import threading
from configHandler import getESPCount
import struct

BROADCAST_IP = "0.0.0.0"  # This is for sending broadcasts, not binding
BROADCAST_PORT = 9999

ESP_LIST = []
EXPECTED_ESP = getESPCount()

# Create a UDP socket for listening to broadcasts
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP socket
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)  # Allow broadcasting

# Bind to all interfaces (0.0.0.0) or a specific local IP address
sock.bind(('0.0.0.0', BROADCAST_PORT))  # Listen on all available interfaces

setup = True
while setup:
    data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes
    strdata = data.decode('utf-8')
    
    if 'Hej hej :)' in strdata:
        # Get last part of the message to extract the port
        data = data[-5:]
        ESP_LIST.append((int(data), addr))

    if len(ESP_LIST) == EXPECTED_ESP:
        setup = False
        sock.close()

def get_broadcast_ip():
    hostname = socket.gethostname()
    local_ip = socket.gethostbyname(hostname)
    
    # Get the subnet mask and broadcast address
    for iface in socket.getaddrinfo(hostname, None):
        if iface[0] == socket.AF_INET:
            addr = iface[4][0]
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                s.connect((addr, 0))
                broadcast_ip = s.getsockname()[0].rsplit('.', 1)[0] + ".255"
                s.close()
                return broadcast_ip
            except:
                pass
    return "255.255.255.255"

def connectToESP(ip: str, port: int):
    # Create a new socket in each thread for communication with the ESP
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # New UDP socket for each thread
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)  # Allow broadcasting
    #sock.bind(('192.168.1.77', port))  # Bind to a specific port for communication with the ESP

    id = port - 10000  # Calculate ESP ID based on the port
    message = "OK ESP_" + str(id)
    print("1")
    # Send "OK" message to ESP to acknowledge
    #sock.sendto(message.encode('utf-8'), (BROADCAST_IP, BROADCAST_PORT))
    sock.sendto(message.encode('utf-8'), (get_broadcast_ip(), BROADCAST_PORT))

    print("2")
    loop = True
    while loop:
        print("fuck")
        data, addr = sock.recvfrom(1024)  # Receive data from ESP
        print(f"ESP {id}: {data.decode('utf-8')}")

    # Close the socket when done with communication
    sock.close()

# Start threads for each ESP device
for esp in ESP_LIST:
    print(f"Connecting to ESP {esp[0]-10000} at {esp[1][0]}")
    thread = threading.Thread(target=connectToESP, args=(esp[1][0], esp[0]), daemon=True)
    thread.start()

while True:
    pass  # Keep the main thread alive so that threads can run

