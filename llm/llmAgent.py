#from ollama import chat, Client
from os import getenv
from configHandler import getValue, mergeDicts
from json import loads
from dotenv import load_dotenv
from openai import NotGiven, OpenAI

class llmAgent:
    """Instantiable object of llm. """
    def __init__(self, systemMsg: dict, returnJson = False, tools = None):
        self.tools = tools
        self.returnJson = returnJson
        self.history = []
        self.systemMsg = systemMsg
        self.maxTokens = int(getValue('llm_settings', 'max_tokens'))
        self.model = getValue('llm_settings', 'model')
        self.remote = getValue('ollama_settings', 'remote')
        self.memory = int(getValue('llm_settings', 'memory'))
        self.llm = getValue('llm_settings', 'source')
        
        if self.llm == 'openai':  # Create openai client
            load_dotenv()
            self.client = OpenAI(api_key=getenv('OPENAI_API_KEY'))
            
            
#        elif self.remote:   # Create ollama client if llm set to ollama & remote
#            self.client = Client(host=getValue('ollama_settings', 'host')) # Create client if set to run remote

    
    def query(self, userPrompt: str):
        usrMsg = {'role': 'user', 'content': userPrompt}
        self.addHistory(usrMsg) # Append usrMsg to history 
        msg = tuple(self.history) # Build message mutable variablers värden är en referens till ett värde. därav måste msg göras till en tuple, sen list för att msg ska peka på ett annat värde, därefter kan man appenda sysmsg
        msg = list(msg)
        msg.append(self.systemMsg)

        if self.returnJson: # response format
            resFormat = {"type": "json_object"}
        else:
            resFormat = NotGiven()

        if self.llm == 'openai': # Run if openai configured
            response = self.client.chat.completions.create(
                model = self.model,
                messages = msg,
                response_format = resFormat,
                tools = self.tools,
                max_tokens = self.maxTokens)
            
            
            if response.choices[0].message.content == None: # Api suger dase
                response.choices[0].message.content = ''

            returnMessage = {'role': response.choices[0].message.role,
                             'content': response.choices[0].message.content}
            
            self.addHistory(returnMessage)
            if self.tools: # Add call to history
                return response.choices[0].message.tool_calls # Return tool call

            if self.returnJson:
                try:
                    content = loads(response.choices[0].message.content)
                    return content # Return message content response as JSON
                except Exception as e:
                    print('Message content not convertable to JSON, returning str: ', e)
            return response.choices[0].message.content # Return message content as str
        
        if self.remote: # Run if ollama set to remote
            response = self.client.chat(
            model = self.model,
            messages = msg,
            tools = self.tools)

            self.addHistory(response['message'])
            if self.returnCall:
                try:
                    json = loads(response['message']) # Return message content as JSON
                    return json
                except Exception as e:
                    print('llm response not convertable to JSON, returning str: ', e)
            return response['message'] # Return message content as str
        
#        else: # Run if ollama set to local
#            response = chat(
#                model = self.model,
#                messages = msg,
#                tools = self.tools)
#            
#            self.addHistory(response['role': 'you', 'content': response['message']['content']])
#            if self.returnCall:
#                try:
#                    json = loads(response['message']) # Return message content as JSON
#                    return json
#                except Exception as e:
#                    print('llm response not convertable to JSON, returning str: ', e)
#            return response['message'] # Return message content as str
        

    def addHistory(self, msg: dict):
        self.history.append(msg)
        if len(self.history) > self.memory:
            self.history.pop(0)