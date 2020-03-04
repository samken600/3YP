ifeq (868,$(ISM))
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    UK_CHANNEL ?= 0 # i.e. use 868MHz
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(UK_CHANNEL)
    UK_PAGE ?= 0 # i.e. use page 0. This corresponds to BPSK. Shouldn't actually matter here: 2 is fine
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_PAGE=$(UK_PAGE)
    SCLK := SPI_CLK_1MHZ
    CFLAGS += -DAT86RF2XX_PARAM_SPI_CLK=$(SCLK)
  endif
endif

# Set a custom channel if needed
ifneq (,$(DEFAULT_CHANNEL))
  ifneq (,$(filter cc110x,$(USEMODULE)))        # radio is cc110x sub-GHz
    CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif

  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif

# Set a custom PAN ID if needed
ifneq (,$(DEFAULT_PAN_ID))
  CFLAGS += -DIEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
endif
