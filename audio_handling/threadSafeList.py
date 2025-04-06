import threading
from collections import deque

class ThreadSafeList:
    """Lista som håller alla ESPs PCM packet i varsit element."""
    def __init__(self, esps: int, maxElementSize: int):
        self._list = deque(maxlen=esps)  # Skapa lista av esps längd med element av listor med längden maxElementSize
        for e in range(self._list.maxlen): 
            self._list.append(deque(maxlen=maxElementSize))
        
        self._elementLocks = [threading.Lock() for _ in range(esps)]  # Ett lås för varje element i pcmListan
        self._indexLock = threading.Lock()  # Lås för get/set index
        self._index = 0  # Vilket element i listan som stt ska använda sig av
        self._newFlags = deque([False] * esps, maxlen=esps)  # Flagga för när ett element har en oläst chunk
        self._condition = threading.Condition()  # För trådar som väntar på flagga

    def getInner(self, index: int):
        """Returnerna första chunken och sätt motsvarande flaggan till false"""
        with self._elementLocks[index]:
            try:
                self._newFlags[index] = False  # chunk läst
                return self._list[index].pop()
            except IndexError:
                return None

    def appendInner(self, index: int, data: list):
        """Lägg till chunk och sätt motsvarande flagga till true"""
        with self._elementLocks[index]:
            try:
                self._list[index].append(data)
                self._newFlags[index] = True  # Ny chunk

                with self._condition:
                    self._condition.notify_all() # Notify STT

            except IndexError:
                print(f"Index {index} out of range.")

    # För att STT inte ska läsa samma chunk flera gånger
    def waitForFlag(self, index: int):
        """Wait until the flag for the given index is true."""
        with self._condition:
            while not self.getFlag(index):
                self._condition.wait()

    def get_index(self):
        with self._indexLock:
            return self._index

    def set_index(self, value):
        with self._indexLock:
            self._index = value

    def getFlag(self, index: int) -> bool:
        """Se om en lista har oläst data"""
        with self._elementLocks[index]:
            return self._newFlags[index]