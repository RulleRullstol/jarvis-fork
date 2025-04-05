import threadSafeList
import listenerSocket

class audioStart:
    def __init__(self):
        self.pcmList = threadSafeList()

    def start(self):
        # Start the listener socket to receive audio data
        listenerSocket.start(self.pcmList)
        #start STT
        #start MISISTT
        