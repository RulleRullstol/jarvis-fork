
import os
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(parent_dir)

import threadSafeList
import threading
import MISISTT
import stt
from ..configHandler import getESPCount
import listenerSocket

class audioStart:
    def __init__(self):
        self.pcmList = threadSafeList(getESPCount(), 8)

    def start(self):
        # Start the listener socket to receive audio data
        listenerSocket.start(self.pcmList)
        #start STT
        sttThread = threading.Thread(target=stt.start, args=(self.pcmList,), daemon=True)
        sttThread.start()
        #start MISISTT
        misisttThread = threading.Thread(target=MISISTT.start, args=(self.pcmList,), daemon=True)
        misisttThread.start()