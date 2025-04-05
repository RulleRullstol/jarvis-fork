
import threadSafeList
import numpy as np
from configHandler import getESPCount


def MISISST(tsl: threadSafeList):
    current_index = 0
    for i in range(getESPCount()):
        samples = tsl.popInner()
        audio_data = np.frombuffer(samples, dtype=np.int16)
        rms = np.sqrt(np.mean(audio_data**2))
        if rms > current_index:
            current_index = rms
    
    tsl.set_index(current_index)