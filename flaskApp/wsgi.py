# run.py

import os
from logger import LOG
from app import app

if __name__ == '__main__':
    LOG.info('running environment: %s', os.environ.get('ENV'))
    app.config['DEBUG'] = False
    app.run() # Run the app
