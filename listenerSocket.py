import socket
import threading
import ipaddress
from configHandler import getESPCount, getBroadcastIp
import struct
 
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
    Fetches the broadcast IP address by modifying the local machine's IP.
    """
    hostname = socket.gethostname()
    
    if (getBroadcastIp() != 'NULL'):
        return getBroadcastIp()
    # Get the local IP address of the machine
    for iface in socket.getaddrinfo(hostname, None):
        if iface[0] == socket.AF_INET:
            local_ip = iface[4][0]
            try:
                # Split the local IP into its parts
                ip_parts = local_ip.split('.')  # Assuming local_ip is like '192.168.10.x'
                
                # Modify the last octet to '255' for the broadcast address
                ip_parts[-1] = '255'
                broadcast_ip = '.'.join(ip_parts)  # Join the parts back into a string
                
                return broadcast_ip
            except Exception as e:
                pass  # Handle any errors, e.g., if the IP format is unexpected
    return "255.255.255.255"  # Fallback if we can't determine the broadcast IP

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
    while True:
        sock.sendto(message.encode('utf-8'), (get_broadcast_ip(), BROADCAST_PORT))
        print("2")

    
    loop = True
    while loop:
        print("fuck")
        data, addr = sock.recvfrom(1024)
        print(f"ESP {id}: {data.decode('utf-8')}")

    sock.close()


########################## Start Connection ##########################
setupESP()

# Start threads for each ESP device
for esp in ESP_LIST:
    print(f"Connecting to ESP {esp[0]-10000} at {esp[1][0]}")
    thread = threading.Thread(target=connectToESP, args=(esp[1][0], esp[0]), daemon=True)
    thread.start()

while True:
    pass  # Keep the main thread alive so that threads can run

