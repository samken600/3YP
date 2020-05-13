import os 
import requests
from flask import jsonify, request, make_response, send_from_directory

from logger import LOG
from app import app

@app.errorhandler(404)
def not_found(error):
    """ error handler """
    LOG.error(error)
#    return make_response(jsonify({'error': 'Not found'}), 404)
    return send_from_directory('static/html', '404.html')


@app.route('/')
def index():
    """ static files serve """
    return send_from_directory('static/html', 'index.html')


@app.route('/<path:path>')
def static_proxy(path):
    """ static folder serve """
    file_name = path.split('/')[-1]
    dir_name = os.path.join('static/html', '/'.join(path.split('/')[:-1]))
    return send_from_directory(dir_name, file_name)

