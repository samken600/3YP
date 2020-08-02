# run server
import sys, os
#from config import dbinfo
import app

import logging

if __name__ == '__main__':
    # run the app boi
    logging.basicConfig(level=logging.INFO)
    app.run()
