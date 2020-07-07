# CoAP to HTTP

It is reccomended that a virtual environment is created here and the packages inside of the [requirements file](requirements.txt) are installed in this. This can be achieved via:

```python
pip3 install -r requirements.txt
```

There is a [configuration file](config.py) that is used to configure what addresses to collect data from. 

When creating a task to collect data, the [pipe script](pipe.py) can just be run using your python environment.

It should be noted that this is only tested with python 3.6.9
