import threadSafeList
import listenerSocket
import threading
import MISISTT
import stt

class audioStart:
    def __init__(self):
        self.pcmList = threadSafeList()

    def start(self):
        # Start the listener socket to receive audio data
        listenerSocket.start(self.pcmList)
        #start STT
        sttThread = threading.Thread(target=stt.start, args=(self.pcmList,), daemon=True)
        sttThread.start()
        #start MISISTT
        misisttThread = threading.Thread(target=MISISTT.start, args=(self.pcmList,), daemon=True)
        misisttThread.start()