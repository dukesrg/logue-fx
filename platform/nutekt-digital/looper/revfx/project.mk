# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/revfx.mk

PROJECT = Looper-revfx

UCSRC =

UCXXSRC = $(PROJECTDIR)/../looper.cpp

UINCDIR =

UDEFS = -DUSER_TARGET_MODULE=k_user_module_revfx -DUSER_TARGET_PLATFORM=k_user_target_nutektdigital

ULIB =

ULIBDIR =
