BUILD := $(addsuffix -build, $(wildcard *fx))
CLEAN := $(addsuffix -clean, $(wildcard *fx))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ./logue-sdk/platform
PLATFORMS = prologue minilogue-xd nutekt-digital

all: $(BUILD)

%-build: %
	@for platform in $(PLATFORMS) ; do \
	$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PLATFORM=$$platform PROJECTDIR=$< ; \
	$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PROJECTDIR=$< clean ; \
	done

clean: $(CLEAN)

%-clean: %
	@for platform in $(PLATFORMS) ; do \
	$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PROJECTDIR=$< clean-remove ; \
	done

.PHONY: all clean
