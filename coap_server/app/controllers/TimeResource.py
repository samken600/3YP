from coapthon.resources.resource import Resource
from coapthon.defines import Codes, Content_types
import datetime

class TimeResource(Resource):
    def __init__(self, name="TimeResource", coap_server=None):
        super(TimeResource, self).__init__(name)
        self.payload = "Time Resource"
    
    def render_GET_advanced(self, request, response):
        response.payload = str(int(datetime.datetime.utcnow().timestamp()))
        response.code = Codes.CONTENT.number
        response.content_type = Content_types["text/plain"]
        return self, response
    
