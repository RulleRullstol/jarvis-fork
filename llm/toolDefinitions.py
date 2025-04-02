from configHandler import getLightNames, getManualNames
from tools.setLights import setLights
from tools.mail import getManual

# Dict of all available functions
functions = {
    'setLights': setLights,
    'getManual': getManual
}

def getTools():
    return [{
        "type": "function",
        "function": {
            "name": "setLights",
            "description": "Sets the properties of lights.",
            "parameters": {
                "type": "object",
                    "properties": {
                    "id": {
                        "type": "string",
                        "description": "The id of the light to be altered. The available IDS are: " + 
                        getLightNames() + ". If no light is specified set id to 'all'." + 
                        "if the user specifies no particular color set the color value to none"
                    },
                    "state": {
                        "type": "boolean",
                        "description": "The on/off state of the light"
                    },
                    "color": {
                        "type": "array",
                        "description": "The RGB color value of the light represented as the aray [R, G, B], each element is an integer and have a range of 0-255",
                        "items": {
                            "type": "number"
                        }
                    },
                    "brightness": {
                        "type": "number",
                        "description": "The brightness of the light, takes a value in the range 0-255"
                    },
                },
                "required": ["id", "state", "color", "brightness"]
            }
        }
    }, {"type": "function",
        "function": {
            "name": "getManual",
            "description": "Sends a manual to the user via email",
            "parameters": {
                "type": "object",
                    "properties": {
                    "id": {
                        "type": "string",
                        "description": "The id of the manuals to be altered. The available IDS are: " + 
                        getManualNames() + ". If no manual is specified set id to 'manual.full'."
                    }
                },
                "required": ["id"]
            }
        }
    }
    ]