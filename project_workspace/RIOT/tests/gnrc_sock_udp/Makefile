include ../Makefile.tests_common

USEMODULE += gnrc_sock_check_reuse
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6
USEMODULE += ps

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=400
endif
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
