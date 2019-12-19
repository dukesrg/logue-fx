PKGARCH = .ntkdigunit
BUILD := $(addsuffix -build, $(wildcard *fx))
CLEAN := $(addsuffix -clean, $(wildcard *fx))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ..
PROJECT = Looper

all: $(BUILD)

%-build: %
#for stock makefiles
#	@cp $(PLATFORMDIR)/$</Makefile $(PLATFORMDIR)/$</project.mk $<
#	@$(MAKE) -C $< PLATFORMDIR=$(PLATFORMDIR) UDEFS=-Dfxtype=$< CSRC=$(PLATFORMDIR)/$</tpl/_unit.c UCXXSRC=../looper.cpp PROJECT=$(PROJECT)-$< LDDIR=$(PLATFORMDIR)/$</ld
#	@cp $(PLATFORMDIR)/$</project.mk $<
#for alt makefiles
	@$(MAKE) -C $(PLATFORMDIR)/$< PLATFORMDIR=$(PLATFORMDIR) UDEFS=-Dfxtype=$< CSRC=$(PLATFORMDIR)/$</tpl/_unit.c UCXXSRC=$(PROJECTDIR)/looper.cpp PROJECT=$(PROJECT)-$< PROJECTDIR=$(PROJECTDIR)/$< LDDIR=$(PLATFORMDIR)/$</ld
#	@$(MAKE) -C $(PLATFORMDIR)/$< PLATFORMDIR=$(PLATFORMDIR) UDEFS=-Dfxtype=$< CSRC=$(PLATFORMDIR)/$</tpl/_unit.c PROJECT=$(PROJECT)-$< PROJECTDIR=$(PROJECTDIR)/$< LDDIR=$(PLATFORMDIR)/$</ld
	@cp $</$(PROJECT)-$<$(PKGARCH) .

clean: $(CLEAN)
	@rm -f $(ARCH)

%-clean: %
	@$(MAKE) -C $(PLATFORMDIR)/$< clean PROJECT=$(PROJECT)-$< PROJECTDIR=$(PROJECTDIR)/$< 
	rm -f $(PROJECT)-$<$(PKGARCH)
#for stock makefiles
#	@rm -f $</Makefile $</project.mk

.PHONY: all clean
