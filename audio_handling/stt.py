
from threadSafeList import ThreadSafeList
from RealtimeSTT import AudioToTextRecorder


def start(pcmList: ThreadSafeList):
    """Stoppa in data i stt grunkan"""
    recorder = AudioToTextRecorder(
        use_microphone=False,
        language='en',
        model='tiny-en',)
    
    while True:
        print(pcmList._index)
        recorder.feed_audio(pcmList.getInner())
        print("Transcription: ", recorder.text())
