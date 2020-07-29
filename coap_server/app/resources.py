from coapthon.server.coap import CoAP
from app.controllers import *
from config import *
#import pdb

class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port))
#        pdb.set_trace()
        try:
            if(MONGO==True):
                self.add_resource('/temp', TempMongoResource.TempMongoResource())
            else:
                self.add_resource('/temp', TempResource.TempResource())
        except NameError:
            self.add_resource('/temp', TempMongoResource.TempMongoResource())
        self.add_resource('/time', TimeResource.TimeResource())
