from json import dumps, loads

def updateCurrentSettings(dataIn: any): # Function(arguments='{"id": "all", "state": true, "color": [0, 0, 0], "brightness": 255}', name='setLights') toolResponse[0].function
    data = {
        'name': dataIn.name,
        'arguments': loads(dataIn.arguments)
    }
    lines = []
    try:
        f = open('currentSettings', 'r')
        lines = f.readlines()
    except Exception as e:
        print('File does not exist, creating new')
        f = open('currentSettings', 'x')
        
    if len(lines) != 0:
        updated = False
        for i, line in enumerate(lines):
            rData = loads(line.replace('\n', ''))
            rArgs = rData['arguments']
            if data['arguments']['id'] == rArgs['id']:
                lines[i] = dumps(data) + '\n'
                updated = True
                break

        if not updated:
            lines.append(dumps(data) + '\n')

    else:
        lines.append(dumps(data) + '\n')

    f.close()
    f = open('currentSettings', 'w')
    f.writelines(lines)
    f.close()

