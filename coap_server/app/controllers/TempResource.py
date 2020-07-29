from coapthon.resources.resource import Resource
from coapthon.defines import Codes, Content_types

class TempResource(Resource):
    def __init__(self, name="TempResource", coap_server=None):
        super(TempResource, self).__init__(name)
        self.payload = "Temperature Resource"
    
    def render_GET_advanced(self, request, response):
        response.payload = self.payload
        response.code = Codes.CONTENT.number
        response.content_type = Content_types["text/plain"]
        return self, response
    
    def render_PUT_advanced(self, request, response):
        self.payload = request.payload
        from coapthon.messages.response import Response
        assert(isinstance(response, Response))
        response.payload = "PUT success"
        response.code = Codes.CHANGED.number
        return self, response

