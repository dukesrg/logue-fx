PKGARCH = *.ntkdigunit
BUILD := $(addsuffix -build, $(wildcard *fx))
CLEAN := $(addsuffix -clean, $(wildcard *fx))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ..

all: $(BUILD)

%-build: %
	@$(MAKE) -C $(PLATFORMDIR)/$< PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$(PROJECTDIR)/$< CSRC=$(PLATFORMDIR)/$</tpl/_unit.c LDDIR=$(PLATFORMDIR)/$</ld
	@cp $</$(PKGARCH) .

clean: $(CLEAN)
	@rm -f $(PKGARCH)

%-clean: %
	@$(MAKE) -C $(PLATFORMDIR)/$< PROJECTDIR=$(PROJECTDIR)/$< clean

.PHONY: all clean
