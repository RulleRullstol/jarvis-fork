import serial
import wave
import struct
import time

# Configuration
SERIAL_PORT = "COM3"  # Change to match your ESP32's port
BAUD_RATE = 921600
OUTPUT_WAV = "recorded_audio.wav"
SAMPLE_RATE = 44100  # Match the ESP32 sample rate
CHANNELS = 1  # Mono audio
BITS_PER_SAMPLE = 16  # 16-bit audio
CHUNK_SIZE = 256  # Number of 16-bit samples per chunk (this is 64 samples)
GAIN = 1 # Volume gain (increase this value to boost the volume)

def increase_volume(samples, gain):
    """Increase the volume by multiplying the samples by a gain factor."""
    amplified_samples = []
    for sample in samples:
        amplified_sample = int(sample * gain)
        # Ensure the sample stays within the valid range for 16-bit PCM
        if amplified_sample > 32767:
            amplified_sample = 32767
        elif amplified_sample < -32768:
            amplified_sample = -32768
        amplified_samples.append(amplified_sample)
    return amplified_samples

def read_audio_from_serial():
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")

    with wave.open(OUTPUT_WAV, "wb") as wav_file:
        wav_file.setnchannels(CHANNELS)
        wav_file.setsampwidth(BITS_PER_SAMPLE // 8)  # 2 bytes per sample
        wav_file.setframerate(SAMPLE_RATE)

        print("Recording... Press Ctrl+C to stop.")

        while True:
            # Ensure enough data is available before reading
            if ser.in_waiting >= CHUNK_SIZE * 2:
                data = ser.read(CHUNK_SIZE * 2)  # 2 bytes per sample for 16-bit audio (256 samples = 512 bytes)

                # Unpack the raw byte data into signed 16-bit integers (little-endian format)
                samples = struct.unpack('<' + 'h' * (len(data) // 2), data)

                # Increase the volume by applying the gain factor
                # amplified_samples = increase_volume(samples, GAIN)

                # Write the amplified audio samples (packed as 16-bit signed integers) to the WAV file
                wav_file.writeframes(struct.pack('<' + 'h' * len(samples), *samples))
            else:
                # Wait until enough data is available
                print(f"Waiting for {CHUNK_SIZE * 2} bytes, currently {ser.in_waiting} bytes available.")
                time.sleep(0.1)  # Small delay before checking again

    ser.close()
    print(f"Saved recording to {OUTPUT_WAV}")

if __name__ == "__main__":
    read_audio_from_serial()
