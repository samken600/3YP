''' controller and routes for leaderboard '''
import os
from flask import request, jsonify
from app import app, mongo

@app.route('/leaderboard', methods=['GET', 'POST', 'DELETE', 'PATCH'])
def leaderboard():
    if request.method == 'GET':
        query = request.args
        print(query.to_dict())
        limit = query.get('limit', None)
        if limit is not None:
            cursor = mongo.db.leaderboard.aggregate( [ { '$sort': {"score": -1} }, { '$limit': int(limit) } ] );
            data = list(cursor)
        else:
            data = mongo.db.leaderboard.find_one(query)
        return jsonify(data), 200

    data = request.get_json()
    if request.method == 'POST':
        if data.get('username', None) is not None and data.get('score', None) is not None and data.get('password', None) is not None:
            data['score'] = float(data['score']);
            if mongo.db.leaderboard.count({'username': data['username']}) == 0:
                mongo.db.leaderboard.insert_one(data)
                return jsonify({'ok': True, 'message': 'User created successfully!'}), 200
            else:
                return jsonify({'ok': False, 'message': 'Username already exists!'}), 409
        else:
            return jsonify({'ok': False, 'message': 'Bad request parameters!'}), 400

    if request.method == 'DELETE':
        if data.get('username', None) is not None and data.get('password', None) is not None:
            db_response = mongo.db.leaderboard.delete_one({'username': data['username'], 'password': data['password']})
            if db_response.deleted_count == 1:
                response = {'ok': True, 'message': 'record deleted'}
            else:
                response = {'ok': True, 'message': 'no record found, confirm password is correct'}
            return jsonify(response), 404
        else:
            return jsonify({'ok': False, 'message': 'Bad request parameters!'}), 400

    if request.method == 'PATCH':
        query = data.get('query', {})
        if query != {}:
            if query.get('username', None) is not None and query.get('password', None) is not None:
                payload = data.get('payload', {})
                payload['score'] = float(payload['score']);
                db_response = mongo.db.leaderboard.update_one(
                                data['query'], {'$max': payload})
                if db_response.matched_count == 0:
                    return jsonify({'ok': False, 'message': 'no record found, confirm password is correct'}), 404
                else:
                    if db_response.modified_count == 0:
                        return jsonify({'ok': True, 'message': 'less than highscore so not updated'}), 200
                    else:
                        return jsonify({'ok': True, 'message': 'record updated'}), 200
            else:
                return jsonify({'ok': True, 'message': 'Bad request parameters!'}), 400
        else:
            return jsonify({'ok': False, 'message': 'Bad request parameters!'}), 400
