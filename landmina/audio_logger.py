import serial
import wave

# Configuration
SERIAL_PORT = "COM4"  # Change to match your ESP32's port
BAUD_RATE = 115200
OUTPUT_WAV = "recorded_audio.wav"
SAMPLE_RATE = 16000
CHANNELS = 1
BITS_PER_SAMPLE = 16
CHUNK_SIZE = 1024

def read_audio_from_serial():
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")

    with wave.open(OUTPUT_WAV, "wb") as wav_file:
        wav_file.setnchannels(CHANNELS)
        wav_file.setsampwidth(BITS_PER_SAMPLE // 8)
        wav_file.setframerate(SAMPLE_RATE)

        print("Recording... Press Ctrl+C to stop.")

        while True:
            data = ser.read(CHUNK_SIZE)
            wav_file.writeframes(data)

    ser.close()
    print(f"Saved recording to {OUTPUT_WAV}")

if __name__ == "__main__":
    read_audio_from_serial()
