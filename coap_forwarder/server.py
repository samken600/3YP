import logging
from coapthon.forward_proxy.coap import CoAP
from coapthon.resources.resource import Resource
from coapthon.defines import Codes, Content_types

class BasicResource(Resource):
    def __init__(self, name="BasicResource", coap_server=None):
        super(BasicResource, self).__init__(name)
        self.payload = "Basic Resource"
    
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

class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port))
#        self.add_resource('/temp', BasicResource())

def main():
    logging.basicConfig(level=logging.INFO)
    server = CoAPServer("::", 5683)
    try:
        server.listen(10)
    except KeyboardInterrupt:
        print("Server shutdown")
        server.close()
        print("Exiting...")

if __name__ == "__main__":
    main()
