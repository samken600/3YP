''' coapthon app with mongo '''

import os
from config import *
from coapthon.server.coap import CoAP
from pymongo import MongoClient

# add mongo url to config

mongo = MongoClient(host       = MONGO_HOST,
                    port       = int(MONGO_PORT),
                    username   = MONGO_USERNAME,
                    password   = MONGO_PASSWORD,
                    authSource = MONGO_AUTH_SOURCE)

db = mongo[MONGO_DBNAME]

from app.resources import CoAPServer

def run():
    server = CoAPServer("::", 5683)
    try:
        server.listen(10)
    except KeyboardInterrupt:
        print("Server shutdown")
        server.close()
        print("Exiting...")
