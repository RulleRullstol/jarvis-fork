import threadSafeList
import listenerSocket
import threading
import MISISTT

class audioStart:
    def __init__(self):
        self.pcmList = threadSafeList()

    def start(self):
        # Start the listener socket to receive audio data
        listenerSocket.start(self.pcmList)
        #start STT
        #start MISISTT
        misisttThread = threading.Thread(target=MISISTT.start, args=(self.pcmList,), daemon=True)
        misisttThread.start()