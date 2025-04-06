import threading
from collections import deque
import pyaudio

CHUNK = 512                     # Number of samples read
FORMAT = pyaudio.paInt16        # Sample format (16-bit integer)
CHANNELS = 1                    # Mono audio
RATE = 16000                    # Sampling rate in Hz (expected by the recorder)
FRAMES_PER_BUFFER = 512         # Number of frames per buffer 1024 8bit = 512 16bit


class ThreadSafeList:
    """Lista som håller alla ESPs PCM packet i varsit element."""
    def __init__(self, esps: int):
        self._list = deque(maxlen=esps)  # Skapa lista av esps längd med element av listor med pyaudio Streams
        for e in range(self._list.maxlen):
            p = pyaudio.PyAudio()
            stream = p.open(
                    format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    output=True,
                    frames_per_buffer=FRAMES_PER_BUFFER,
                )
            self._list.append(stream)
        self._elementLocks = [threading.Lock() for _ in range(esps)]  # Ett lås för varje element i pcmListan
        self._indexLock = threading.Lock()  # Lås för get/set index
        self._index = 0  # Vilket element i listan som stt ska använda sig av

    def readStream(self, index: int):
        """Returnerna första chunken och sätt motsvarande flaggan till false"""
        with self._elementLocks[index]:
            try:
                return self._list[index].read(CHUNK) # pyaudio
            except Exception as e:
                print(f"Error: Failed to read from pyaudio stream: {e}")
                return False

    def writeStream(self, index: int, data: bytes):
        """Lägg till chunk och sätt motsvarande flagga till true"""
        with self._elementLocks[index]:
            try:
                self._list[index].write(data)
            except OSError as e:
                print(f"Error: Failed to write to pyaudio stream: {e}")

    def get_index(self):
        with self._indexLock:
            return self._index

    def set_index(self, value):
        with self._indexLock:
            self._index = value