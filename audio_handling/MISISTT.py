
import threadSafeList
import numpy as np

import os
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(parent_dir)

from configHandler import getESPCount


def start(tsl: threadSafeList):
    current_index = 0
    while True:
        for i in range(getESPCount()):
            samples = tsl.popInner()
            audio_data = np.frombuffer(samples, dtype=np.int16)
            rms = np.sqrt(np.mean(audio_data**2))
            if rms > current_index:
                current_index = rms

        tsl.set_index(current_index)