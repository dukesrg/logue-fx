BUILD := $(addsuffix -build, $(wildcard *fx))
CLEAN := $(addsuffix -clean, $(wildcard *fx))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ./logue-sdk/platform/nutekt-digital

all: $(BUILD)

%-build: %
	@$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$<

clean: $(CLEAN)

%-clean: %
	@$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$< clean-remove

.PHONY: all clean
