import threading
from collections import deque

class ThreadSafeList:
    """Lista som håller alla ESPs PCM packet i varsit element."""
    def __init__(self, esps: int, maxElementSize: int):
        self._list = deque(maxlen=esps)  # Skapa lista av esps längd med element av listor med längden maxElementSize
        for e in range(self._list.maxlen): 
            self._list.append(deque(maxlen=maxElementSize))
        
        self._elementLocks = [threading.Lock() for _ in range(esps)] # Ett lås för varje element i pcmListan
        self._indexLock = threading.Lock() # Lås för get/set index
        self._index = 0  # Vilket element i listan som stt ska använda sig av

    def getInner(self):
        with self._elementLocks[self._index]:
            try:
                return self._list[self._index][0]
            except IndexError:
                print(f"Error: Buffer not loaded yet... maybe")
                return None
    
    def appendInner(self, index, data: list):
        with self._elementLocks[index]:
            try:
                self._list[index].append(data)
            except IndexError:
                print(f"Index {index} out of range.")

    def get_index(self):
        with self._indexLock:
            return self._index

    def set_index(self, value):
        with self._indexLock:
            self._index = value

