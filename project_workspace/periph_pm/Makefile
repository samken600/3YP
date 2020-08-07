BOARD ?= slstk3401a
RIOTBASE ?= $(CURDIR)/../RIOT/
include $(RIOTBASE)/tests/Makefile.tests_common

# method `fflush()` is not defined for MSP-430 (#6445 will fix this)
BOARD_BLACKLIST := chronos msb-430h msb-430 telosb wsn430-v1_3b wsn430-v1_4 z1

FEATURES_OPTIONAL += periph_rtc
FEATURES_OPTIONAL += periph_gpio_irq

DISABLE_MODULE += auto_init_at86rf2xx

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += at86rf212b

include $(RIOTBASE)/Makefile.include
