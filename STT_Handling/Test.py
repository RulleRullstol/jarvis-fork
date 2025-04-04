from RealtimeSTT import AudioToTextRecorder

def process_text(text):
    print(text)

if __name__ == '__main__':
    recorder = AudioToTextRecorder(
        language='en',
        model='tiny',
        device='cpu',
        input_device_index=0
    )

    while True:
        recorder.text(process_text)