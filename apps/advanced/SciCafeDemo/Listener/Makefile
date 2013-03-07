#-*-Makefile-*- vim:syntax=make
#
# --------------------------------------------------------------------
#  The developer must define at least SOURCES and APPMOD in this file
#
#  In addition, PROJDIR and OSW must be defined, before including 
#  the main Makefile at ${OSW}/src/make/Makefile
# --------------------------------------------------------------------

# Sources are all project source files, excluding system files
SEAL_SOURCES = main.sl

# Module is the name of the main module built by this makefile
APPMOD = SealApp

# --------------------------------------------------------------------
# Set the key variables
PROJDIR = $(CURDIR)
ifndef OSW
  OSW = ../../../..
endif

# Include the main makefile
include ${OSW}/src/make/Makefile
