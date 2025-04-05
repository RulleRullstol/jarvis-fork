import threadSafeList
import socket
from configHandler import getBroadcastIp

class listener:
    def __init__(self, pcmList: threadSafeList, writeIndex: int, espID: str):
        self.pcmList = pcmList                          # Lista där vi stoppar PCM datan
        self.writeIndex = writeIndex                    # Index to write to in pcmList

        self.espID = espID                              # ex. "ESP_0"
        self.handshakeMSG = 'Hej hej :)'                # Meddelande från ESP
        self.handshakeACK = 'OK ' + espID               # Skicka till ESP

        self.targetAddress = None                       # Set by handshakeESP()
        self.targetPort = None                          # Set by handshakeESP()
        self.broadcastAddress = getBroadcastIp(self)    # broadcast address vi ska prata med esp på
        self.BROADCAST_PORT = 9999
        self.SAMPLE_RATE = 22050
        self.BITS_PER_SAMPLE = 16
        self.CHUNK_SIZE = 1024
        
    def handshakeESP(self):
        """Listens for the handshake message on the broadcast IP and port, and sets the target address and port."""
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            sock.bind(('', self.BROADCAST_PORT))  # Bind to the broadcast port

            while True:
                data, addr = sock.recvfrom(1024)  # Receive data from the socket
                if data.decode('utf-8') == self.handshakeMSG:
                    self.targetAddress, self.targetPort = addr
                    sock.sendto(self.handshakeACK.encode('utf-8'), addr)  # Send acknowledgment
                    break
        
    def getBroadcast(self) -> str:
        """Fetches the broadcast IP address."""
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


    def start(self):
        # Initialize audio stream
        pass

    def stop(self):
        # Stop audio stream
        pass

    def process_audio(self, data):
        # Process audio data
        pass