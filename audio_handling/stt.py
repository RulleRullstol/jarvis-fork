
import threadSafeList
from RealtimeSTT import AudioToTextRecorder


def start(pcmList: threadSafeList):
    """Stoppa in data i stt grunkan"""
    while True:
        pcmList[pcmList.get_index()].pop()
        