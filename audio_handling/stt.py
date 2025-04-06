import struct
from threadSafeList import ThreadSafeList
from RealtimeSTT import AudioToTextRecorder

def start(pcmList: ThreadSafeList):
    """Feed audio chunks to the recorder and handle transcription."""

    def onNewText(transcription):
        """callback för recorder.text"""
        print("Transcription: ", transcription)

    # Initialize the recorder with the callback
    recorder = AudioToTextRecorder(
        use_microphone=False,
        language='en',
        model='tiny.en',
        buffer_size=512,
        spinner=False,
        enable_realtime_transcription=True,
        on_realtime_transcription_update=onNewText
    )

    while True:
        print("chunk fed")
        pcmList.waitForFlag(pcmList.get_index())
        samples = pcmList.getInner(pcmList.get_index())
        samples_bytes = bytearray(struct.pack('h' * len(samples), *samples))
        recorder.feed_audio(samples_bytes)
        recorder.text(on_transcription_finished=onNewText)