# Flask Server

It is reccomended that a virtual environment is created here and the packages inside of the [requirements file](requirements.txt) are installed in this. This can be achieved via:

To run the server in local debug mode, use `bash python3 run.py`. The `wsgi.py` can be used a WSGI entry point for a deployed server. 

In an industry version of this, the details inside `config.py` should be obfuscated for security reasons. 

This server requires a MongoDB instance running locally. For setup, see the following links:

* Setting up MongoDB - https://www.howtoforge.com/tutorial/install-mongodb-on-ubuntu/
* Deploying the server on Ubuntu 18.04 - https://www.digitalocean.com/community/tutorials/how-to-serve-flask-applications-with-gunicorn-and-nginx-on-ubuntu-18-04
