from RealtimeSTT import AudioToTextRecorder

def process_text(text):
    print(text)

if __name__ == '__main__':
    recorder = AudioToTextRecorder(
        language='sv',
        model='base',
        device='cpu',
        input_device_index=2,
        spinner=False
    )

    while True:
        recorder.text(process_text)
