import socket
import threading
import time
import wave
import struct
from threadSafeList import ThreadSafeList
from configHandler import getESPCount, getBroadcastIp

BROADCAST_PORT = 9999

ESP_LIST = []
EXPECTED_ESP = getESPCount()

OUTPUT_WAV = "recorded_audio.wav"
SAMPLE_RATE = 16000
CHANNELS = 1
BITS_PER_SAMPLE = 16
CHUNK_SIZE = 1024

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
        print(f"Searching for ESP's. {len(ESP_LIST)} of {EXPECTED_ESP} found")
        time.sleep(1)
        data, addr = sock.recvfrom(1024)
        strdata = data.decode('utf-8')
        
        if 'Hej hej :)' in strdata:
            data = data[-5:]
            if not ((int(data), addr) in ESP_LIST):
                ESP_LIST.append((int(data), addr))

        if len(ESP_LIST) == EXPECTED_ESP:
            setup = False
            sock.close()
    print("All ESP's have been found")


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

def connectToESP(pcmList: ThreadSafeList, port: int):
    """
    Connects to one ESP. Done in a seperate thread, once for each ESP
    to be connected.
    """
    # Create a new socket in each thread for communication with the ESP
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    id = port - 10000  # Calculate ESP ID based on the port
    message = "OK ESP_" + str(id)
    for i in range(5):
        sock.sendto(message.encode('utf-8'), (get_broadcast_ip(), BROADCAST_PORT))
        print(f"Returning digital handshake ({i*5}%)")
    print("Returning digital handshake (100%)\n")
    
    local_ip = socket.gethostbyname(socket.gethostname())
    print(f"Binding to local ip {local_ip} on port {port}")
    sock.close()
    time.sleep(2)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind((local_ip, port))
    print("Binding successful\nEnabling datastream capture")
    ############################################# DEBUG
    
    while True:
        data = sock.recvfrom(1024)[0]
        samples = struct.unpack('<' + 'h' * (len(data) // 2), data)
        pcmList.appendInner(id, samples)
    

def recordESP(sock):
    with wave.open(OUTPUT_WAV, "wb") as wav_file:
        wav_file.setnchannels(CHANNELS)
        wav_file.setsampwidth(BITS_PER_SAMPLE // 8)
        wav_file.setframerate(SAMPLE_RATE)
        
        
        sample_count = 0
        sample_stock = []
        sampling = True
        file_length = 1000
        
        while sampling:
            data, addr = sock.recvfrom(1024)
            sample_count += 1
            samples = struct.unpack('<' + 'h' * (len(data) // 2), data)
            sample_stock.append(samples)

            if sample_count >= file_length :
                sampling = False
            else:
                print(f"Collecting samples ({sample_count//(file_length/100)}%)")
            #time.sleep(len(data) / (SAMPLE_RATE / 2))
        for i in range(len(sample_stock)):
            wav_file.writeframes(struct.pack('<' + 'h' * len(sample_stock[i]), *sample_stock[i]))
            print(f"Writing to file ({i//(len(sample_stock)/100)}%)")
        print("Done Writing!!!")

########################## Start Connection ##########################

def listenerSocketStart(pcmList: ThreadSafeList):
    print(f"Broadcast ip found:         {get_broadcast_ip()}")
    print(f"Own ip found:               {socket.gethostbyname(socket.gethostname())}")

    setupESP()

    # Start threads for each ESP device
    for esp in ESP_LIST:
        print(f"Connecting to ESP {esp[0]-10000} at {esp[1][0]}")
        thread = threading.Thread(target=connectToESP, args=(pcmList, esp[0]), daemon=True)
        thread.start()

    while True:
        pass  # Keep the main thread alive so that threads can run

