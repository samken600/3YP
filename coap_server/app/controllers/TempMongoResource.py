from coapthon.resources.resource import Resource
from coapthon.defines import Codes, Content_types
from app import db
import json, datetime

class TempMongoResource(Resource):
    def __init__(self, name="TempMongoResource", coap_server=None):
        super(TempMongoResource, self).__init__(name)
        self.payload = "Temperature Resource with Mongo"
    
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

    def render_POST_advanced(self, request, response):
        from coapthon.messages.response import Response
        assert(isinstance(response, Response))

        self.payload = request.payload
        data = json.loads(request.payload)
        item = {}
        if data.get('temp', None) is not None and data.get('hwaddr', None) is not None:
            item['temp'] = float(data['temp'])
            data['hwaddr'] = str(data['hwaddr']).upper().strip()

            node = db.nodes.find_one({'hwaddr': data['hwaddr']})
            if node is None:
                node_num = db.nodes.count()
                node_dict = { 'hwaddr': data['hwaddr'], 'node_num': int(node_num) }
                db.nodes.insert_one(node_dict)
                item['node'] = int(node_num)
            else:
                item['node'] = int(node['node_num'])

            if data.get('time', None) is not None:
                item['time'] = datetime.datetime.utcfromtimestamp(int(data['time']))
                item['true_time'] = True
            else:
                item['time'] = datetime.datetime.utcnow()
                item['true_time'] = False

            if db.temp.count({'time': item['time'], 'node': item['node']}) == 0:
                db.temp.insert_one(item)
                response.payload = json.dumps({'ok': True, 'message': 'POST Success'})
                response.code = Codes.CREATED.number            
            else:
                response.payload = json.dumps({'ok': False, 'message': 'Data already exists'})
                response.code = Codes.NOT_ACCEPTABLE.number
        else:
            response.payload = json.dumps({'ok': False, 'message': 'Bad request params'})
            response.code = Codes.BAD_REQUEST.number

                
        return self, response

