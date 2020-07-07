# 3YP

## [Project Workspace](project_workspace)

See the [README](project_workspace/README.md) inside for details on setting up RIOT. Note that the RIOT and openocd sub-repositories are managed with git subtrees, NOT submodules. Ensure the correct commands are used when updating these modules.

* Border router files to run on SAMR30 Xplained-Pro boards (`/gnrc_border_router`)

* Final CoAP server to run on PCBs (`/3yp_node_coap`)

* Testing program to run on PCBs (`/3yp_node_testing`)

* NanoCoAP server to run on SAMR21 boards with DS18 temperature sensor (`/nanocoap_server`)

* Modified RIOT directory (`/RIOT`)

## [PCB Files](pcb/)

* Parts list for PCB
* [Eagle project files](pcb/EAGLE/) (see `/projects/3YP PCB Rev2A` for project and `/libraries` for parts). Can use this as the working directory for EAGLE. 

## [CoAP to HTTP query script](coap-http)

* Bridge between CoAP and HTTP

* Scheduled script runs every 10 minutes (`coap-http/pipe.py`)

## [Flask Application](flaskApp/)

* Flask webserver files
* HTML and CSS (`/app/static`)
* Temperature HTTP API (`/app/controllers`)
* Graphing webpage/dashboard (`/plotlydash`)
