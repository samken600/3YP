''' all controllers for various collections of database '''
import os
import glob

__all__ = [os.path.basename(f)[:-3]
    for f in glob.glob(os.path.dirname(__file__) + "/*.py")]
# Import all routes in files inside controller directory. Only need to import controllers module
