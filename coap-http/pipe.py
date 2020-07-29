from coapthon.client.helperclient import HelperClient
from datetime import datetime
import requests
import pdb

URL = "https://samken600.me/temp"

from config import hosts
port = 5683
path = 'temp'
clients = []

for host in hosts:
    client = HelperClient(server=(host['ip'], port))
    hw_array = host['ip'].split(":")[-4:]
    hw_array[0] = "%x" % (int(hw_array[0], 16) + 512)
    hwaddr = "".join(hw_array)
    hwaddr = ":".join(hwaddr[i:i+2] for i in range(0, len(hwaddr), 2)).upper()
    clients.append({'client': client, 'number': host['number'], 'hwaddr': hwaddr})

responses = []
for client in clients:
    now = datetime.utcnow()
    response = client['client'].get(path=path, timeout=15) # pretty sure timeout is in seconds
    if response is not None:
        print(response.pretty_print())
        responses.append({'time': now, 'response': response, 'number': client['number'], 'hwaddr': client['hwaddr']})
    client['client'].stop()

for response in responses:
    if response['response'].code is 69: # 69 = content
        data = {
                    'time': response['time'].timestamp(),
                    'hwaddr': response['hwaddr'],
                    'temp': int(response['response'].payload)/100
               }
        print(data)
        r = requests.post(url=URL, json=data)

