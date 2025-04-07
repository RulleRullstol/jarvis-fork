import numpy as np
import whisper
import threading
import time
from threadSafeList import audioBuffers
from whisper import DecodingOptions

def start(buffer: audioBuffers):
    model = whisper.load_model("tiny.en")
    audio_data = bytearray()
    lock = threading.Lock()  # Ensure thread-safe access to audio_data

    def process_audio():
        nonlocal audio_data
        while True:
            with lock:
                if len(audio_data) >= 8000 * 2:  # Process 0.5 seconds of audio
                    pcm_data = np.frombuffer(audio_data[:8000 * 2], dtype=np.int16).astype(np.float32) / 32768.0
                    audio_data = audio_data[8000 * 2:]  # Remove processed data from buffer

                    # Generate Mel spectrogram
                    mel = whisper.log_mel_spectrogram(pcm_data).to(model.device)

                    # Decode audio
                    options = DecodingOptions(fp16=False)
                    try:
                        result = whisper.decode(model, mel, options)
                        print("Transcription:", result.text)
                    except Exception as e:
                        print("Error during transcription:", e)

            time.sleep(0.01)  # Avoid busy-waiting

    # Start a thread to process audio in real-time
    threading.Thread(target=process_audio, daemon=True).start()

    while True:
        try:
            packet = buffer.readBytes(buffer.getIndex())
            if packet:
                with lock:
                    audio_data.extend(packet)
        except Exception as e:
            print("Error reading from buffer:", e)
        time.sleep(0.01)  # Simulate real-time packet arrival