import requests
import json

from configHandler import getLights, getDictValues, mergeDicts, getValue, getHAheaders

def setLights(id: str, state: bool, color: list, brightness: int):
    debug = False
    # Turn input into [['light.hall', 'on', [255, 160, 60], 255]]

    lights = getLights() # Iterera över konfigurerade lampor för att hitta korrekt lampa
    lconf= {}  # Lampans inställningar
    
    a = [id, state, color, brightness]
    actions = [a]
    if a[0].lower() == "all":
        actions = derefrenceAll(a, lights)
    for action in actions:
        for light in lights: #
            values = getDictValues(light)
            lconf = mergeDicts(light)
            if action[0] in values:
                # POST till HA
                payload = {}
                url = getValue('home_assistant_settings', 'url')
                payload['entity_id'] = lconf['ha_id']
                if action[1]: # If state on
                    if lconf['color'] and action[2] != None: payload['rgb_color'] = tuple(action[2]) # Set color if configured
                    if lconf['brightness'] and action[2] != None: payload['brightness'] = action[3] # Set brightness if configured
                    url += lconf['uri_on'] # Append uri
                else: # If state off
                    url += lconf['uri_off']
                # Gammala koden för post men headers hämtas från config.ini
                response = requests.post(url, headers=getHAheaders(), data=json.dumps(payload))
                if debug:
                    print('payload: ',payload)
                    print('url: ', url)
                    
                    if response.status_code != 200:
                        print("Successfully executed: ", action)
                    else:
                        print("Failed to execute:", action, " :: ", {response.status_code})
                        print("Response:", response.text)
                break    

def derefrenceAll(action: list, lights: list):
    actions = []
    lconf = {}
    for light in lights:
        lconf = mergeDicts(light)
        actions.append([lconf['ha_id'], action [1], action[2], action[3]])
    
    return actions