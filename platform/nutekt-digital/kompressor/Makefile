BUILD := $(addsuffix -build, $(wildcard *fx))
CLEAN := $(addsuffix -clean, $(wildcard *fx))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ..

all: $(BUILD)

%-build: %
	@$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$<
#	@cp $</$(PKGARCH) .

clean: $(CLEAN)
	@rm -f $(PKGARCH)

%-clean: %
	@$(MAKE) -f fx.mk PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$< clean

.PHONY: all clean
