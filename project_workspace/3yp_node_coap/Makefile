# name of your application
APPLICATION = nanocoap_server

# If no BOARD is found in the environment, use this default:
BOARD = 3yp-node
DEFAULT_CHANNEL = 0 # 868MHz

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT/

# include to use shell and print ips
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += printf_float

# include to interface with d18 temp sensor
USEMODULE += si7006

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default # MUST HAVE ONE ROUTER
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp

# Add rpl routing
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
CFLAGS += -DGNRC_RPL_DEFAULT_NETIF=7
# CFLAGS += -DGNRC_MAC_RX_QUEUE_SIZE=16
# CFLAGS += -DGNRC_MAC_TX_QUEUE_SIZE=16
# CFLAGS += -DGNRC_LWMAC_IPC_MSG_QUEUE_SIZE=32

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

USEMODULE += nanocoap_sock

# include this for nicely formatting the returned internal value
USEMODULE += fmt

# include mac layer to use
USEMODULE += gnrc_lwmac
CFLAGS += -DGNRC_LWMAC_ENABLE_LOW_POWER=0

# Comment this out to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP = 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
