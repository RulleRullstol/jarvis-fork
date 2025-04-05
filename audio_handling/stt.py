
import threadSafeList
from RealtimeSTT import AudioToTextRecorder


def start(pcmList: threadSafeList):
    """Stoppa in data i stt grunkan"""
    recorder = AudioToTextRecorder(use_microphone=False)
    while True:
        recorder.feed_audio(pcmList.getInner())
        print("Transcription: ", recorder.text())
