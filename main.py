from tools.toolDefinitions import getTools, functions
from llm.llmAgent import llmAgent
from llm import systemMsgs
from json import loads
from protocols.settingsTracking import updateCurrentSettings
import threading

def getUsrInput(usrInput: list): # Greppa user input. arg är referens till input variabel som kollas i loopen
    while True:
        request = input('User: ')
        usrInput.append(request)

def parseToolCalls(funcs):  # Köra funktioner från llm
    for call in funcs:
        callsToUpdate.append(call.function) # Append to functioncall list for current settings logic
        try:
            func = functions[call.function.name]
            args = loads(call.function.arguments)
            funcThread = threading.Thread(target=func, kwargs= args, daemon=True)
            funcThread.start()
        except Exception as e:
            print(f'Error calling function: {e}')


# Setup response agent
talkAgent = llmAgent(systemMsgs.openai_fast_msg, False)
toolAgent = llmAgent(systemMsgs.openai_lights_msg, True, getTools())

# Setup for keeping track of current settings and tools to call
callsToUpdate = [] # List of tool calls to be passed to potentially slow updateThread
updateThread = any # the update thread
toolResponse = [] # Tool responses from llm

# Setup for getting user input async
usrInput = [] # Måste vara lista för att språket är dåligt
usrInputThread = threading.Thread(target = getUsrInput, args = [usrInput], daemon=True)
usrInputThread.start()

while True:
    if len(usrInput) != 0:
        for _input in usrInput:
            chatResponse = talkAgent.query(_input) # chat response
            print('Jarvis: ', chatResponse)
            tools = toolAgent.query(_input)
            if tools != None:
                toolResponse.append(tools)
        usrInput.clear()

    if len(toolResponse) != 0:
        for call in toolResponse:
            parseToolCalls(call) # Run tool calls
        toolResponse = []

    if len(callsToUpdate) > 0: # Update current settings. Run if calls is not empty
        if (isinstance(updateThread, threading.Thread) and not updateThread.is_alive) or not isinstance(updateThread, threading.Thread): # Is previous updateThread done? Start new.
            updateThread = threading.Thread(target = updateCurrentSettings, args = [callsToUpdate.pop(0)], daemon=True)
            updateThread.start()