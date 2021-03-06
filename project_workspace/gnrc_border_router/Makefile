# name of your application
APPLICATION = gnrc_border_router

# If no BOARD is found in the environment, use this default:
BOARD = samr30-xpro
DEFAULT_CHANNEL = 0

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# use ethos (ethernet over serial) for network communication and stdio over
# UART, but not on native, as native has a tap interface towards the host.
ifeq (,$(filter native,$(BOARD)))
  # ethos baudrate can be configured from make command
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
else
  TERMFLAGS += -z [::1]:17754
endif
GNRC_NETIF_NUMOF := 2

# SLIP legacy compatibility
# Uncomment the lines below if you want to use SLIP with this example and don't
# forget to comment the lines above for ethos.
#ifeq (,$(SLIP_UART))
# set default (last available UART)
#SLIP_UART="UART_DEV(UART_NUMOF-1)"
#endif
#ifeq (,$(SLIP_BAUDRATE))
# set default
#SLIP_BAUDRATE=115200
#endif

#GNRC_NETIF_NUMOF := 2
#INCLUDES += -I$(CURDIR)
#CFLAGS += -DSLIP_UART=$(SLIP_UART)
#CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)
# Include SLIP package for IP over Serial communication
#USEMODULE += slipdev

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# CFLAGS += -DCONFIG_IEEE802154_DEFAULT_TXPOWER=-6

# include UHCP client
USEMODULE += gnrc_uhcpc

# Optionally include RPL as a routing protocol. When includede gnrc_uhcpc will
# configure the node as a RPL DODAG root when receiving a prefix.
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl

# CFLAGS += -DGNRC_MAC_TX_QUEUE_SIZE=16
# CFLAGS += -DGNRC_MAC_RX_QUEUE_SIZE=16
# CFLAGS += -DGNRC_LWMAC_IPC_MSG_QUEUE_SIZE=32


# include mac layer to use
#USEMODULE += gnrc_lwmac
CFLAGS += -DGNRC_LWMAC_ENABLE_LOW_POWER=0

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1
TAP ?= tap0
IPV6_PREFIX ?= 2001:db8::/64

ifeq (native,$(BOARD))
TERMDEPS += uhcpd-daemon

.PHONY: uhcpd-daemon

uhcpd-daemon: host-tools
	$(RIOTTOOLS)/uhcpd/bin/uhcpd $(TAP) $(IPV6_PREFIX) &
else
# We override the `make term` command to use ethos
TERMPROG ?= sudo sh $(RIOTTOOLS)/ethos/start_network.sh
TERMFLAGS ?= $(PORT) $(TAP) $(IPV6_PREFIX)

# We depend on the ethos host tools to run the border router, we build them
# if necessary
TERMDEPS += host-tools
endif

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
