ifeq (868,$(ISM))
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    UK_CHANNEL ?= 0 # i.e. use 868MHz
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(UK_CHANNEL)
    UK_PAGE ?= 2 # i.e. use page 2. Page 0 corresponds to BPSK. Page 2 is O-QPSK I think. Shouldn't actually matter here but it does: use 2
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_PAGE=$(UK_PAGE)
    SCLK := SPI_CLK_5MHZ
    CFLAGS += -DAT86RF2XX_PARAM_SPI_CLK=$(SCLK)
  endif
endif

# Set a custom channel if needed
ifndef CONFIG_KCONFIG_MODULE_IEEE802154
  ifneq (,$(DEFAULT_CHANNEL))
    ifneq (,$(filter cc110x,$(USEMODULE)))        # radio is cc110x sub-GHz
      CFLAGS += -DCONFIG_CC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
    endif

    ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
    else                                          # radio is IEEE 802.15.4 2.4 GHz
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
    endif
  endif

  # Set a custom PAN ID if needed
  ifneq (,$(DEFAULT_PAN_ID))
    CFLAGS += -DCONFIG_IEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
  endif
endif
