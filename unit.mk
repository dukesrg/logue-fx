include $(PROJECTDIR)/project.mk
include $(PLATFORMDIR)/../logue-sdk.mk

POST_ALL: package move

clean-remove: clean remove

move:
	@mv -f $(PROJECTDIR)/$(PKGARCH) $(PROJECTDIR).$(PKGEXT)

remove:
	@rm -f $(PROJECTDIR).$(PKGEXT)
