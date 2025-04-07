import struct
from threadSafeList import audioBuffers
from RealtimeSTT import AudioToTextRecorder
import threading
import time

def sttStart(pcmList: audioBuffers): # PCMlista är numera lista av pyaudio streams
    """Feed audio chunks to the recorder and handle transcription."""
    # Audio stream configuration constants
    CHUNK = 512                 # Måste vara mindre än 512

    # Initialize the audio-to-text recorder without using the microphone directly
    # Since we are feeding audio data manually, set use_microphone to False

    # Event to signal when to stop the threads
    stop_event = threading.Event()
    recorder = AudioToTextRecorder(
        use_microphone=False,
        language='en',
        model='tiny.en',
        buffer_size=512, # Måste vara 512
        sample_rate=16000,
        spinner=False,
        #enable_realtime_transcription=True,
        #on_realtime_transcription_update=onNewText
    )

    def feed_audio_thread(pcmList: audioBuffers):
            """Thread function to read audio data, save it to a .wav file, and feed it to the recorder."""
            try:
                print(f"{threading.current_thread().name}: Reading buffers...")
                while not stop_event.is_set():
                    time.sleep(0.01)  # Sleep for a short duration to avoid busy waiting
                    # Read audio data from the buffer
                    data = pcmList.readBytes(pcmList.getIndex(), CHUNK)  # Read CHUNK bytes
                    if data:= None:
                        recorder.feed_audio(data)
            except Exception as e:
                print(f"feed_audio_thread encountered an error: {e}")
            finally:
                print("Audio feeding thread exiting.")


    def recorder_transcription_thread():
            """Thread function to handle transcription and process the text."""
            def process_text(full_sentence):   
                """Callback function to process the transcribed text."""
                print(f"{threading.current_thread().name} Transcribed text:", full_sentence, "")
                # Check for the stop command in the transcribed text
                if "stop recording" in full_sentence.lower():
                    print("Stop command detected. Stopping threads...")
                    stop_event.set()
                    recorder.abort()
            try:
                while not stop_event.is_set():
                    # Get transcribed text and process it using the callback
                    recorder.text(process_text)
            except Exception as e:
                print(f"{threading.current_thread().name}: transcription_thread encountered an error: {e}")
            finally:
                print(f"{threading.current_thread().name}: thread exiting.")

    """Start here"""
    print(f"{threading.current_thread().name}: Starting audio feeding and transcription threads...")
    # Create and start the audio feeding thread
    audio_thread = threading.Thread(target=feed_audio_thread, args=(pcmList,))
    audio_thread.daemon = False    # Ensure the thread doesn't exit prematurely
    audio_thread.start()

    # Create and start the transcription thread
    transcription_thread = threading.Thread(target=recorder_transcription_thread)
    transcription_thread.daemon = False    # Ensure the thread doesn't exit prematurely
    transcription_thread.start()

    # Wait for both threads to finish
    audio_thread.join()
    transcription_thread.join()

    print("Recording and transcription have stopped.")
    recorder.shutdown()