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
    clients.append({'client': client, 'number': host['number']})


responses = []
for client in clients:
    now = datetime.now()
    response = client['client'].get(path=path, timeout=15) # pretty sure timeout is in seconds
    print(response.pretty_print())
    responses.append({'time': now, 'response': response, 'number': client['number']})
    client['client'].stop()

for response in responses:
    if response['response'].code is 69: # 69 = content
        data = {
                    'time': response['time'].isoformat(),
                    'node': response['number'],
                    'temp': int(response['response'].payload)/100
               }
        print(data)
        r = requests.post(url=URL, json=data)

