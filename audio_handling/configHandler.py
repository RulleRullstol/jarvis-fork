import configparser
from typing import Literal
'''Diskombobulatorn. Den diskombobulerar.'''

def getConf():
    config = configparser.ConfigParser()
    config.read('config.ini')
    return config

def getESPCount():
    conf = getConf()
    count = conf['esp_general']['count']
    return int(count)

def getBroadcastIp():
    conf = getConf()
    return conf['esp_general']['broadcast_ip']

def getLights():
    '''Returns all light settings as a list of dicts'''
    lights = []
    data = []
    conf = getConf()
    for sec in conf.sections():             
        if sec.startswith('light'):
            for atr in conf[sec]:
                item:dict = {atr: conf[sec][atr]}
                data.append(item)
            lights.append(data)
            data = []        
    return lights

def getSpeakers():
    '''Returns all speaker settings as a list of dicts'''
    speakers = []
    data = []
    conf = getConf()
    for sec in conf.sections():
        if sec.startswith('speaker'):
            for atr in conf[sec]:
                item:dict = {atr: conf[sec][atr]}
                data.append(item)
            speakers.append(data)
            data = []        
    return speakers

def getValue(secName: str, value: str):
    '''Return specific value from config as STR'''
    conf = getConf()
    return conf[secName].get(value)



def getDictValues(keys: list):
    values: list = []
    for key in keys:
        key = list(key.values())[0]
        values.append(key)
    return values

def mergeDicts(list: list):
    out = {}
    for d in list:
        out.update(d)
    return out

def getHAheaders():
    '''Returns headers for HA POSTs'''
    conf = getConf()
    headers = dict(conf['ha_headers'])
    headers['authorization'] += " " + getValue('home_assistant_settings', 'token')
    return headers

def getLightNames():
    '''Returns a stringified list of all light aliases'''
    lightIds = []
    for light in getLights():
        lightIds.append(light[0]['alias'])

    return str(lightIds)

def getManuals():
    '''Returns all manual settings as a list of dicts'''
    manuals = []
    data = []
    conf = getConf()
    for sec in conf.sections():             
        if sec.startswith('manual'):
            for atr in conf[sec]:
                item:dict = {atr: conf[sec][atr]}
                data.append(item)
            manuals.append(data)
            data = []        
    return manuals

def getManualNames():
    '''Returns a stringified list of all light aliases'''
    manualIds = []
    for manual in getManuals():
        manualIds.append(manual[0]['alias'])

    return str(manualIds)

