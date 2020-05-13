''' flask app with mongo '''
import os
import json
import datetime
from bson.objectid import ObjectId
from flask import Flask
from flask_pymongo import PyMongo

# Allow to interact with ObjectId (_id) and datatime objects (timestamp)
class JSONEncoder(json.JSONEncoder):
    ''' extend json-encoder class'''

    def default(self, o):
        if isinstance(o, ObjectId):
            return str(o)
        if isinstance(o, datetime.datetime):
            return str(o)
        return json.JSONEncoder.default(self, o)

# Init app
app = Flask(__name__, instance_relative_config=True)

# Load config
app.config.from_object('config')

# add mongo url to flask config, so that flask_pymongo can use it to make connection
mongo = PyMongo(app)

# use the modified encoder class to handle ObjectId & datetime object while jsonifying the response.
app.json_encoder = JSONEncoder


with app.app_context():
    # Load views
    from app import views

    # Import Dash application
    from app.plotlydash.dashboard import create_dashboard
    app = create_dashboard(app)

    # Compile CSS
#    from app.assets import compile_assets
#    compile_assets(app)
    

# Ensure instance folder exists
try:
    os.makedirs(app.instance_path)
except OSError:
    pass

# include controllers
from app.controllers import *
