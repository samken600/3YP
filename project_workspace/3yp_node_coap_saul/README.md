nanocoap saul example
=======================

This application shows how you can use saul with a nanocoap server in order to serve temperature readings to a client.
It uses the GNRC network stack through RIOT's
[sock API](http://doc.riot-os.org/group__net__sock.html).

Usage
=====

This application was designed to work with read hardware. Should work on the SAMR30-Xplained pro boards as well as SAMR21 boards with some minor tweaks.

To compile for a specific target board, i.e. the samr30-xpro board, use

```
$ make all BOARD=samr30-xpro
```

In order to communicate with the board wirelessly, the [gnrc\_border\_router](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router) example should be put on another node using the same hardware. If using a SAMR30 board, or other sub GHz board,make sure that the channel is set correctly if you are a user within europe by adding the following line to the makefile

```
DEFAULT_CHANNEL = 0
```

It should also be noted that this line should be removed if operating in the 2.4GHz band.

Once this is all up and running, you should notice that running

```
$ nib neigh
```

on the border router shows the global ipv6 address of the target node, starting with 2001:db8::

If you don't see this, it means that the boards are not connecting to each other. Check both boards are configured to be using the same technologies on the same, valid channels in the Makefiles.

Another note is that when you have multiple boards plugged into a single machine, you need to specify the target serial. You can use the `make list-ttys` command to list connected serial device. This can then be used to flash the hardware, as shown below:

```
$ make term BOARD=samr30-xpro SERIAL=ATML2722081800000603
make -C ethos
make[2]: Nothing to be done for 'all'.
make -C uhcpd
make[2]: Nothing to be done for 'all'.
make -C sliptty
make[2]: Nothing to be done for 'all'.
sudo sh /home/skendall/work/3yp/project_workspace/RIOT/dist/tools/ethos/start_network.sh /dev/ttyACM0 tap0 2001:db8::/64
net.ipv6.conf.tap0.forwarding = 1
net.ipv6.conf.tap0.accept_ra = 0
----> ethos: sending hello.
----> ethos: activating serial pass through.
----> ethos: hello reply received
nib neigh
nib neigh
fe80::1 dev #7 lladdr 6E:83:7C:AC:A9:CB router REACHABLE GC
2001:db8::e805:bc73:5270:6e8c dev #6 lladdr EA:05:BC:73:52:70:6E:8C router REACHABLE GC
```

The global address of the end node in this case is "2001:db8::e805:bc73:5270:6e8c".

Testing
=======

The CoAP server exposes 6 different resources:

* `/.well-known/core`: returns the list of available resources on the server.
This is part of the CoAP specifications. It works only with GET requests.

* `/led/`: will set LED state depending on value in payload. For off, <=0.
  For on, >=1. It works with either POST or PUT requests.

* `/riot/board`: returns the name of the board running the server. It works
  only with GET requests.

* `/temp`: returns a json containing a "d" field and a "u" field. The numerical
  reading is stored in the "d" field and the unit in the "u" field. This should be 
  degrees celsius.

* `/txpower`: sets tx power of transceiver on chip. This assumes that the netif
  set in the Makefile is the netif the radio driver is on. Only works with PUT.

There are multiple external CoAP clients you can use to easily test the server.
One good option is lib-coap, which is available as an ubuntu package.

libcoap CLI
-----------

(replace "2001:db8::e805:bc73:5270:6e8c" with your global address)

The routing for this is setup automatically by the border router, so no need to specify the interface!

* Get the name of the board:
```
    # coap-client -m get coap://[2001:db8::e805:bc73:5270:6e8c]/riot/board
```

* Update the LED state:
```
    # coap-client -m put coap://[2001:db8::e805:bc73:5270:6e8c]/led -e 0
```

* Get the current temperature:
```
    # coap-client -m get coap://[2001:db8::e805:bc73:5270:6e8c]/temp
```

CoAPthon3
-----------

This is a python implementation of CoAP that can be found [(here](https://github.com/Tanganelli/CoAPthon3). This can be used to implement a CoAP server or a send requests acting as a client.

Without some modifications, this will only accept utf-8 encoded tokens and cause a server crash if this is violated. There is a pull request that patches this ([here](https://github.com/Tanganelli/CoAPthon3/pull/20)) although this has not yet been merged due to some tests failing.
