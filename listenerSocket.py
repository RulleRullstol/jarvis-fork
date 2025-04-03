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


def setupESP():
    """
    Searches for all expected ESP for first connection.
    """
    setup = True
    while setup:
        data, addr = sock.recvfrom(1024)
        strdata = data.decode('utf-8')
        
        if 'Hej hej :)' in strdata:
            data = data[-5:]
            if not ((int(data), addr) in ESP_LIST):
                ESP_LIST.append((int(data), addr))
            print(len(ESP_LIST))

        if len(ESP_LIST) == EXPECTED_ESP:
            setup = False
            sock.close()

def get_broadcast_ip():
    """
    Fetches general broadcast ip for handshake.
    """
    hostname = socket.gethostname()
    
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
    """
    Connects to one ESP. Done in a seperate thread, once for each ESP
    to be connected.
    """
    # Create a new socket in each thread for communication with the ESP
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    id = port - 10000  # Calculate ESP ID based on the port
    message = "OK ESP_" + str(id)
    print("1")
    sock.sendto(message.encode('utf-8'), (get_broadcast_ip(), BROADCAST_PORT))

    print("2")
    loop = True
    while loop:
        print("fuck")
        data, addr = sock.recvfrom(1024)
        print(f"ESP {id}: {data.decode('utf-8')}")

    sock.close()

# Start threads for each ESP device
for esp in ESP_LIST:
    print(f"Connecting to ESP {esp[0]-10000} at {esp[1][0]}")
    thread = threading.Thread(target=connectToESP, args=(esp[1][0], esp[0]), daemon=True)
    thread.start()


########################## Start Connection ##########################
setupESP()

while True:
    pass  # Keep the main thread alive so that threads can run

