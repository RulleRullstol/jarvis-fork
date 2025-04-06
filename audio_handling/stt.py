import struct
import queue
from threadSafeList import ThreadSafeList
from RealtimeSTT import AudioToTextRecorder
from collections import deque
import threading
import time  # Import the time module

def start(pcmList: ThreadSafeList):
    """Feed audio chunks to the recorder and handle transcription."""
    print(f"{threading.current_thread().name}: Recorder init")
    transcription_queue = deque()  # Use deque for better performance

    def transcription_finished_callback(transcription):
        """Callback to handle finished transcriptions."""
        transcription_queue.append(transcription)  # Add transcription to the queue

    # Initialize the recorder with the callback
    recorder = AudioToTextRecorder(
        use_microphone=False,
        language='en',
        model='tiny.en',
        buffer_size=512,
        handle_buffer_overflow=True,
        spinner=False,
        on_transcription_start=transcription_finished_callback
    )
    
    def feed_audio(pcmList: ThreadSafeList):
        """Threaded audio feeding."""
        last_time = None  # Variable to store the last execution time
        true_counter = 0  # Counter for how many times the condition is True

        while True:
            # Wait for the flag to be True
            pcmList.waitForFlag(pcmList.get_index()) # kanske för långsam


            # Calculate time difference if this is not the first occurrence
            if last_time is not None:
                last_time = current_time  # Update the last execution time
            
            true_counter += 1  # Increment the counter

            # Print the average time difference every 300 occurrences
            if true_counter == 31:
                current_time = time.time()
                time_diff = (current_time - last_time) * 1000 if last_time is not None else 0
                print(f"{threading.current_thread().name}: Samples equivalent 1 sec read, took : {time_diff:.3f} ms")
                true_counter = 0
                last_time = current_time

            # Process the audio
            samples = pcmList.getInner(pcmList.get_index())
            samples_bytes = bytearray(struct.pack('h' * len(samples), *samples))
            recorder.feed_audio(samples_bytes)

    try:
        recorder.start()  # Start the recorder's processing pipeline
        print(f"{threading.current_thread().name}: Recorder initialized and started.")

        # Start the audio feeding thread
        audio_thread = threading.Thread(target=feed_audio, args=(pcmList,), daemon=True)
        audio_thread.start()

        # Main loop for transcription handling
        while True:
            print(recorder.text())

    except KeyboardInterrupt:
        print("Shutting down...")
    finally:
        recorder.shutdown()  # Ensure proper cleanup
