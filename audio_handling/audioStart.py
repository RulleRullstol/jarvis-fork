
import os
import sys
import time

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(parent_dir)
from configHandler import getESPCount

import threadSafeList
import threading
import MISISTT
import stt
import listenerSocket

class audioStart:
    def __init__(self):
        print(getESPCount())
        self.pcmList = threadSafeList.ThreadSafeList(getESPCount(), 8)
        time.sleep(1)

    def audioStartStart(self):
        # Start the listener socket to receive audio data
        listenerMomaThread = threading.Thread(target=listenerSocket.listenerSocketStart, args=(self.pcmList,), daemon=True)
        listenerMomaThread.start()
        #start STT
        sttThread = threading.Thread(target=stt.start, args=(self.pcmList,), daemon=True)
        sttThread.start()
        #start MISISTT
        misisttThread = threading.Thread(target=MISISTT.misisttStart, args=(self.pcmList,), daemon=True)
        misisttThread.start()

ast = audioStart()
ast.audioStartStart()
while True:
    pass