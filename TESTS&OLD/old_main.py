
import sys
import os

# Includes path to parent directiry
parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)

from llm import jarvis as j
from tools.lightMain import setLights

jarvis = j.Jarvis()

while True:
    _input = input("You: ")

    if _input.lower() == "exit":
        break
    
    chat_response = jarvis.fast_response(_input) 
    print("Jarvis: ", chat_response["message"])

    light_response = []
    speaker_response = []
    
    if chat_response["needs_commands"] != None:
        for action in chat_response["needs_commands"]:
            if action.lower() == "light":
                light_response = jarvis.lights_response()    
            elif action.lower() == "speaker":
                speaker_response = jarvis.speaker_response()  
    
    if len(light_response) != 0:
        setLights(light_response)

        
#
#   if len(speaker_response) != 0:
#       speakerMain.main(speaker_response)
    
    