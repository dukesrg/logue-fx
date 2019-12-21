# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/delfx.mk

PROJECT = Looper-delfx

UCSRC =

UCXXSRC = $(PROJECTDIR)/../looper.cpp

UINCDIR =

UDEFS = -DUSER_TARGET_MODULE=k_user_module_delfx -DUSER_TARGET_PLATFORM=k_user_target_nutektdigital

ULIB =

ULIBDIR =
