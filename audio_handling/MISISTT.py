
from threadSafeList import ThreadSafeList
import numpy as np
import struct

import os
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(parent_dir)

from configHandler import getESPCount


def misisttStart(tsl: ThreadSafeList):
    current_index = 0
    while True:
        for i in range(getESPCount()):
            samples = tsl.readStream(i)
            if samples != None:
                samples_bytes = bytearray(struct.pack('h' * len(samples), *samples)) # Konvertera till bytearray
                audio_data = np.frombuffer(samples_bytes, dtype=np.int16)
                rms = np.sqrt(np.mean(np.abs(audio_data**2)))
                if rms > current_index:
                    current_index = i

        tsl.set_index(current_index)