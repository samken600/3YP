import logging
from coapthon.forward_proxy.coap import CoAP

class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port))

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
