# run.py

import sys, os

from logger import LOG
from app import app

# Port variable to run the server on.
PORT = os.environ.get('PORT') # Not actually used...


if __name__ == '__main__':
    LOG.info('running environment: %s', os.environ.get('ENV'))
    app.config['DEBUG'] = os.environ.get('ENV') == 'development' # Debug mode if development env
    app.run(host='0.0.0.0', port=int(4000)) # Run the app
