import struct
from threadSafeList import ThreadSafeList
from RealtimeSTT import AudioToTextRecorder
import threading
import pyaudio

def start(pcmList: ThreadSafeList): # PCMlista är numera lista av pyaudio streams
    """Feed audio chunks to the recorder and handle transcription."""
    # Audio stream configuration constants
    CHUNK = 512                 # Number of audio samples per buffer

    # Initialize the audio-to-text recorder without using the microphone directly
    # Since we are feeding audio data manually, set use_microphone to False

    # Event to signal when to stop the threads
    stop_event = threading.Event()
    recorder = AudioToTextRecorder(
        use_microphone=False,
        language='en',
        model='tiny.en',
        buffer_size=512,
        spinner=False,
        #enable_realtime_transcription=True,
        #on_realtime_transcription_update=onNewText
    )

    def feed_audio_thread(pcmList: ThreadSafeList):
            """Thread function to read audio data and feed it to the recorder."""
            p = pyaudio.PyAudio()
            # Open an input audio stream with the specified configuration
            p.Stream
            stream = pcmList._list[pcmList.get_index()] # Det här är vår pyaudio stream
            try:
                print("Reading stream")
                while not stop_event.is_set():
                    # Read audio data from the stream (in the expected format)
                    data = stream.read(CHUNK)
                    if data != False:   # returerar False om stream är tom
                        recorder.feed_audio(data)
                    # Feed the audio data to the recorder
            except Exception as e:
                print(f"feed_audio_thread encountered an error: {e}")
            finally:
                # Clean up the audio stream
                stream.stop_stream()
                stream.close()
                p.terminate()
                print("Audio stream closed.")

    def recorder_transcription_thread():
            """Thread function to handle transcription and process the text."""
            def process_text(full_sentence):   
                """Callback function to process the transcribed text."""
                print("Transcribed text:", full_sentence)
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
                print(f"transcription_thread encountered an error: {e}")
            finally:
                print("Transcription thread exiting.")

    """Start here"""
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