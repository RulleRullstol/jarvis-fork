import threading

class ThreadSafeList:
    """Lista som håller alla ESPs PCM packet i varsit element."""
    def __init__(self, maxElementSize):
        self._list = [] # pcm data 
        self._lock = threading.Lock()
        self._index = 0  # Index

    def popInner(self, index: int):
        with self._lock:
            try:
                return self._list[index].pop(0)
            except IndexError:
                print(f"Index {index} out of range or list[index] is not a list.")
                return None
    
    def appendInner(self, index, data: list):
        with self._lock:
            try:
                self._list[index].extend(data)
            except IndexError:
                print(f"Index {index} out of range.")

    def debubPrintInnerListSize(self, index):
        with self._lock:
            try:
                print(f"Inner list size at index {index}: {len(self._list[index])}")
            except IndexError:
                print(f"Index {index} out of range.")


    def get_index(self):
        with self._lock:
            return self._index

    def set_index(self, value):
        with self._lock:
            self._index = value
