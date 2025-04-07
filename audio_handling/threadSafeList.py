import threading
from collections import deque

class audioBuffers:
    """Lista som håller alla ESPs PCM packet i varsit element."""
    def __init__(self, esps: int, bufferSize: int = 10 * 1024): # buffersize (bytes)
        self.bufferList = [deque(maxlen=bufferSize) for _ in range(esps)]  # Skapa lista av esps längd med element av listor med pyaudio Streams
        self.bufferLocks = [threading.Lock() for _ in range(esps)]  # Ett lås för varje element i buffer listan
        self.indexLock = threading.Lock()  # Lås för get/set index
        self.index = 0  # Vilket element i listan som stt ska använda sig av

    def readBytes(self, index: int, numberOfBytes: int = 1024) -> bytes:
        """Returnerna bytes från en buffer"""
        with self.bufferLocks[index]:
            if len(self.bufferList[index]) < numberOfBytes:
                return None
            data = bytes([self.bufferList[index].popleft() for _ in range(numberOfBytes)]) # popar numberOfBytes
            return data

    def writeBytes(self, index: int, data: bytes):
        """Lägg till bytes i en buffer"""
        with self.bufferLocks[index]:
            try:
                self.bufferList[index].extend(data)  # Lägger till varje byte som element
            except Exception as e:
                print(f"Failed to write to buffer: {e}")

    def getIndex(self):
        with self.indexLock:
            return self.index

    def setIndex(self, value):
        with self.indexLock:
            self.index = value