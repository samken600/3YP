# CoAP Server

It is reccomended that a virtual environment is created here and the packages inside of the [requirements file](requirements.txt) are installed in this. This can be achieved via:

```
$ python3 -m venv /flaskEnv
$ source /flaskEnv/bin/activate
$ pip3 install -r requirements.txt
```

To run the server, use `python3 run.py` 

This server requires a MongoDB instance running locally by default. For setup, see the following links:

* Setting up MongoDB - https://www.howtoforge.com/tutorial/install-mongodb-on-ubuntu/
* Deploying the server on Ubuntu 18.04 - https://www.digitalocean.com/community/tutorials/how-to-serve-flask-applications-with-gunicorn-and-nginx-on-ubuntu-18-04

If you do not plan to do this, the MONGO option in `config.py` can be disabled. Instead of storing readings in a database, the server will store the last reading received and will return it upon a GET request to `/temp`.
