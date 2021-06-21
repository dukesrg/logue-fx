BUILD := $(addsuffix -build, $(wildcard *fx))
BUILD += $(addsuffix -build, $(wildcard *osc))
CLEAN := $(addsuffix -clean, $(wildcard *fx))
CLEAN += $(addsuffix -clean, $(wildcard *osc))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ./logue-sdk/platform
PLATFORMS = prologue minilogue-xd nutekt-digital

all: $(BUILD)

%-build: %
	@for platform in $(PLATFORMS) ; do \
	$(MAKE) -f unit.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PROJECTDIR=$< clean ; \
	$(MAKE) -f unit.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PLATFORM=$$platform PROJECTDIR=$< ; \
	done

clean: $(CLEAN)

%-clean: %
	@for platform in $(PLATFORMS) ; do \
	$(MAKE) -f unit.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PROJECTDIR=$< clean-remove ; \
	done

.PHONY: all clean
