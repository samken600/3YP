# 3YP

## [Project Workspace](project_workspace)

See the [README](project_workspace/README.md) inside for details on setting up RIOT. Note that the RIOT and openocd sub-repositories are managed with git subtrees, NOT submodules. Ensure the correct commands are used when updating these modules.

* Border router files to run on SAMR30 Xplained-Pro boards (`/gnrc_border_router`)

* CoAP server to run on PCBs (`/3yp_node_coap`)

* Testing program to run on PCBs (`/3yp_node_testing`)

* NanoCoAP server to run on SAMR21 boards with DS18 temperature sensor (`/nanocoap_server`)

* Modified RIOT directory (`/RIOT`)

* CoAP server using SAUL, designed to work with different hardware, providing access to LEDs, a temperature sensor and TX power (`/3yp_node_coap_saul`)

* CoAP server and client for automatically PUTting sensor readings and self timestamping packets, designed to work with [coap\_server](coap_server/) and [coap\_forwarder](coap_forwarder) (`3yp_node_coap_saul_puts`)

* Power management testing programs, with and without networking (`periph_pm`, `periph_pm_networking`)

## [PCB Files](pcb/)

* Parts list for PCB
* [Eagle project files](pcb/EAGLE/) (see `/projects/3YP PCB Rev2A` for project and `/libraries` for parts). Can use this as the working directory for EAGLE. 

## [CoAP to HTTP query script](coap-http/)

* Bridge between CoAP and HTTP

* Scheduled script runs every 10 minutes (`coap-http/pipe.py`)

## [Flask Application](flaskApp/)

* Flask webserver files
* HTML and CSS (`/app/static`)
* Temperature HTTP API (`/app/controllers`)
* Graphing webpage/dashboard (`/plotlydash`)

## [Coap Forwarder](coap_forwarder/)

* Using CoAPthon3 with binary token patch
* Acts as forwarder between IPv6 GNRC network and IPv4 outer network
* Only role is to forward packets outside of network

## [Coap Server](coap_server/)

* Using CoAPthon3 with binary token patch
* Provides API to POST temperature data to and GET the current time
* Interfaced with MongoDB in `config.py` and so data served via the flask webserver
