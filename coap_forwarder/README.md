# CoAP Forwarder

It is reccomended that a virtual environment is created here and the packages inside of the [requirements file](requirements.txt) are installed in this. This can be achieved via:

```
$ python3 -m venv /flaskEnv
$ source /flaskEnv/bin/activate
$ pip3 install -r requirements.txt
```

To run the server, use `python3 server.py` 

All this application does is forward on CoAP packets sent to it to whatever is in the Uri-Proxy option field of the packet. This is useful if either you or your server does not have IPv6 connectivity due to your ISP or if you want to apply controls to the traffic passing through.
