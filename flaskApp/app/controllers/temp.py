''' controller and routes for temp '''
import os
from flask import request, jsonify
from app import app, mongo
import datetime
import dateutil.parser

@app.route('/temp', methods=['GET', 'POST']) #, 'DELETE', 'PATCH'])
def temp():
    if request.method == 'GET':
        query = request.args
        print(query.to_dict())
        limit = query.get('limit', None)
        if limit is not None:
            cursor = mongo.db.temp.aggregate( [ { '$sort': {"time": 1} }, { '$limit': int(limit) } ] );
            data = list(cursor)
        else:
            data = mongo.db.temp.find_one(query)
        return jsonify(data), 200

    data = request.get_json()
    if request.method == 'POST':
        if data.get('temp', None) is not None and data.get('hwaddr', None) is not None:
            data['hwaddr'] = str(data['hwaddr']).upper().strip()
            data['temp'] = float(data['temp'])

            node = mongo.db.nodes.find_one({'hwaddr': data['hwaddr']})
            if node is None:
                node_num = mongo.db.nodes.count()
                node_dict = { 'hwaddr': data['hwaddr'], 'node_num': int(node_num) }
                mongo.db.nodes.insert_one(node_dict)
                data['node'] = int(node_num)
            else:
                data['node'] = int(node['node_num'])
            data.pop('hwaddr')


            if data.get('time', None) is not None:
                data['time'] = datetime.datetime.utcfromtimestamp(int(data['time']))
                data['true_time'] = True
            else:
                data['time'] = datetime.datetime.utcnow()
                data['true_time'] = False

            if mongo.db.temp.count({'time': data['time'], 'node': data['node']}) == 0:
                mongo.db.temp.insert_one(data)
                return jsonify({'ok': True, 'message': 'Data added sucessfully'}), 200
            else:
                return jsonify({'ok': False, 'message': 'Data already exists for this node with this timestamp'}), 409
        else:
            return jsonify({'ok': False, 'message': 'Bad request parameters!'}), 400

#    if request.method == 'DELETE':
#        if data.get('username', None) is not None and data.get('password', None) is not None:
#            db_response = mongo.db.temp.delete_one({'username': data['username'], 'password': data['password']})
#            if db_response.deleted_count == 1:
#                response = {'ok': True, 'message': 'record deleted'}
#            else:
#                response = {'ok': True, 'message': 'no record found, confirm password is correct'}
#            return jsonify(response), 404
#        else:
#            return jsonify({'ok': False, 'message': 'Bad request parameters!'}), 400
#
#    if request.method == 'PATCH':
#        query = data.get('query', {})
#        if query != {}:
#            if query.get('username', None) is not None and query.get('password', None) is not None:
#                payload = data.get('payload', {})
#                payload['score'] = float(payload['score']);
#                db_response = mongo.db.temp.update_one(
#                                data['query'], {'$max': payload})
#                if db_response.matched_count == 0:
#                    return jsonify({'ok': False, 'message': 'no record found, confirm password is correct'}), 404
#                else:
#                    if db_response.modified_count == 0:
#                        return jsonify({'ok': True, 'message': 'less than highscore so not updated'}), 200
#                    else:
#                        return jsonify({'ok': True, 'message': 'record updated'}), 200
#            else:
#                return jsonify({'ok': True, 'message': 'Bad request parameters!'}), 400
#        else:
#            return jsonify({'ok': False, 'message': 'Bad request parameters!'}), 400
