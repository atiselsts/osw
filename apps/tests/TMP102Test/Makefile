#-*-Makefile-*- vim:syntax=make
#
# --------------------------------------------------------------------
#	Makefile for the sample application
#
#  The developer must define at least SOURCES and APPMOD in this file
#
#  In addition, PROJDIR and OSW must be defined, before including 
#  the main Makefile at ${OSW}/src/make/Makefile
# --------------------------------------------------------------------

# Sources are all project source files, excluding system files
SOURCES = main.c

# Module is the name of the main module buit by this makefile
APPMOD = TMP102Test

# --------------------------------------------------------------------
# Set the key variables
PROJDIR = $(CURDIR)
ifndef OSW
  OSW = $(PROJDIR)/../../..
endif

# Include the main makefile
include ${OSW}/src/make/Makefile
