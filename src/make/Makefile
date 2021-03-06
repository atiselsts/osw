#-*-Makefile-*- vim:syntax=make
#
# Copyright (c) 2013 OSW. All rights reserved.
# Copyright (c) 2008-2012 the MansOS team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#  * Redistributions of source code must retain the above copyright notice,
#    this list of  conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#
# Makefile for all platforms, invoked from the user project makefile.
#
# The user project must define or verify the following variables:
#	OSW = path/to/OpenSCIware/root/directory/
#	PROJDIR = path/to/project/directory/
#	APPMOD  = software-application-or-module-name
#	SOURCES = list of project source files 
# 
# The target specifies the hardware platform and optional hw modules. 
# For example, 
#	make telosb 
# will create a telosb compatible application;
#	make telosb seepoga 
# will compile for telosb platform with seepoga LCD and sensors module.
#
# 
# Available actions:
#
# make <platform> [module]*
#	- makes for the specified platform and optional module(s)
#
# make <platform> [module]* run
#	- makes the application and executes it (e.g. on pc platform)
#
# make <platform> upload
#	- makes as needed and uploads the application or module
#
# Available platforms are defined by the other makefiles:
# 	Makefile.platforms 
#	platforms/Makefile.<platform>
#	modules/Makefile.<module>
#

#===== Directories =====
SRC = $(OSW)/src

OUTDIR = $(PROJDIR)/build
BUILDDIR = $(PROJDIR)/build

#===== Files

MAKEFILE_PLATFORMS = $(SRC)/make/Makefile.platforms
MAKEFILE_CONFIG = $(SRC)/make/Makefile.config
MAKEFILE_OPTIONS = $(SRC)/make/Makefile.options
MAKEFILE_GCC = $(SRC)/make/Makefile.gcc
MAKEFILE_HIL = $(SRC)/make/Makefile.hil

#===== verbosity
VERBOSE ?= $(V)

ifneq (,$(findstring $(OPT_VERBOSE), $(MAKECMDGOALS)))
VERBOSE = 1
endif

ifneq (,$(findstring $(OPT_QUIET), $(MAKECMDGOALS)))
VERBOSE = 
endif

ifneq ($(VERBOSE),)
_QUIET =
_ECHOP = @true
else
_QUIET = @
_ECHOP = @echo
endif

#===== make tools =====
Print                  = $(_ECHOP) "  "
Copy                   = cp
MakeDir                = mkdir -p
Delete                 = rm -fr
List                   = cat

UNAME := $(shell uname)
SHORT_UNAME := $(shell uname | cut -b -6)


#===== Include all necessary and optional files

# user specific options
-include ~/.osw.siteopts

# Declare targets and options
include $(MAKEFILE_OPTIONS)

# Detect the correct platfrom
include $(MAKEFILE_PLATFORMS)

# Include configuration specific options
include $(MAKEFILE_CONFIG)

#===== optimization
OPTIMIZE ?= yes

ifneq (,$(findstring $(OPT_OPTIMIZE), $(MAKECMDGOALS)))
	OPTIMIZE = yes
endif
ifeq (y,$(OPTIMIZE))
	OPTIMIZE = yes
endif
ifeq (y,$(OPTIMIZE_SIZE_EXTRA))
	OPTIMIZE_SIZE_EXTRA = yes
endif

#===== Select and include the makefiles according the target platform
ifneq ($(PLATFORM),)
  # dirty hack: this is needed for cygwin
  PROJDIR = .

  # setup output directories
  OUTDIR = $(PROJDIR)/build/$(PLATFORM)

  # Include platform specific Makefile
  include $(SRC)/make/platforms/Makefile.$(PLATFORM)

  ifeq ($(ARCH),)
  $(error "Selected platform $(PLATFORM) does not define MCU architecture!")
  endif

  # Include architecture specific Makefile
  include $(SRC)/make/arch/Makefile.$(ARCH)
endif

# Decide whether to build or to skip this target for this platform
ifneq ("", "$(PLATFORM_ONLY)")
ifeq ("","$(filter $(PLATFORM), $(PLATFORM_ONLY))")
PLATFORM_ACTION=platform_skip
endif
endif
ifneq ("", "$(PLATFORM_EXCLUDE)")
ifneq ("","$(filter $(PLATFORM), $(PLATFORM_EXCLUDE))")
PLATFORM_ACTION=platform_skip
endif
endif

# for now, no support for threads on PC
ifeq ("y", "$(USE_THREADS)")
ifeq (pc,$(PLATFORM))
PLATFORM_ACTION=platform_skip
endif
endif

# Decide the correct build action
ifneq ("","$(SEAL_SOURCES)")
PLATFORM_ACTION ?= $(BUILDDIR)/config.saved platform_build_seal
endif
PLATFORM_ACTION ?= $(BUILDDIR)/config.saved platform_build 

#===== Tools =====

SEAL = $(OSW)/tools/parser/main.py --path $(OSW)

MEMDUMP = $(SRC)/make/scripts/memdump.py
STACKDUMP = $(SRC)/make/scripts/stackdump.py

# change this to your MSPsim installation location
MSPSIM=/home/atis/work/contiki/contiki-2.5/tools/mspsim/mspsim.jar
JAVA = java


ifeq ($(UNAME),Darwin)
  # apple wants special handling...
  MOTELIST = $(SRC)/make/scripts/motelist.apple
else
ifeq ($(SHORT_UNAME),CYGWIN)
  # cygwin too
  MOTELIST = $(SRC)/make/scripts/motelist.exe
  CFLAGS + =-DCYGWIN=1
else
ifeq ($(SHORT_UNAME),MINGW3)
  # mingw
  MOTELIST=$(SRC)/make/scripts/motelist.exe
  CFLAGS += -DCYGWIN=1
  BSLPORT ?= "$(shell $(MOTELIST) -c | cut -f2 -d, )"
else
  # the normal case 
  MOTELIST ?= $(SRC)/make/scripts/motelist
endif
endif
endif

SENSORLIST ?= python $(SRC)/make/scripts/sensorlist.py

# Set up the correct programmer port
BSLPORT ?= "$(shell $(MOTELIST) -c | xargs -0 echo | xargs echo | cut -f2 -d, )"

# serial port application
SERIAL ?= $(OSW)/tools/serial/ser.py

# this is for listening (use PROG_BAUDRATE for programming!)
BAUDRATE ?= 38400

#===== Source files =====

ifeq ($(USE_REPROGRAMMING),y)
# put user and system code in separate sections
# TODO: move these platform dependent constants to platform files!
LDFLAGS += "-Wl,--section-start=.text=0x5000,--section-start=.usertext=0xf000,--section-start=.start=0x4000,--defsym=_reset_vector__=0x4000"
endif

LINKER_SCRIPT = $(SRC)/make/scripts/link.py

# include the file where platform independent sources are defined
include $(MAKEFILE_HIL)

#===== Includes =====
INCLUDES += -I$(SRC)/include
INCLUDES += -I$(SRC)
INCLUDES += -I$(SRC)/kernel
INCLUDES += -I$(SRC)/lib
INCLUDES += -I$(SRC)/lib/processing
INCLUDES += -I$(SRC)/net
INCLUDES += -I$(SRC)/hil
INCLUDES += -I$(SRC)/arch/$(ARCH)
INCLUDES += -I$(SRC)/platforms/$(PLATFORM)
INCLUDES += -I$(SRC)/chips

#=================== SEAL support
GENERATED_SOURCES = $(SEAL_SOURCES:%.sl=$(BUILDDIR)/%.c)

#===== Sorting out the Objs and Sources =====

PSOURCES += $(PSOURCES-y) $(PSOURCES-yes)

APP_OBJS = $(SOURCES:%.c= $(OUTDIR)/%.o)

# add platform sources and objs
PSRC2 = $(subst $(OSW),$(OUTDIR),$(PSOURCES))
POBJS = $(PSRC2:%.c= %.o)

OBJS = $(APP_OBJS) $(POBJS)
OBJDIRS = $(dir $(OBJS))

# ===== Targets =====
.PHONY: all $(PLATFORM_TARGET) build saveplatform objDirs motelist serial sensorlist \
	stackdump stackdump-build clean cleanall

all: build

#
# The next three targets are for deciding what to actually do.
# - platform_build: build the executable and save this target
# - platform_build_seal: Call make recursively.
#   Needed because SEAL compiler (re)generates config file
# - platform_skip: avoid building this project for this platform
#
platform_build: build saveplatform
	$(Print) ""

# SEAL code compiled successfully, now build C sources.
#ifeq ($(SHORT_UNAME),MINGW3)

# Sorry for this mess - needed to compile at all on MinGW...  
#platform_build_seal: $(GENERATED_SOURCES)
#	(cd $(BUILDDIR);\
#	rm -rf ./build;\
#	MAKECMDGOALS="$(TARGET)" $(MAKE) $(TARGET);\
#	$(MakeDir) $(TARGET);\
#	$(Copy) build/$(TARGET)/$(SEAL_EXECUTABLE) $(TARGET)/$(EXECUTABLE))

#else

# the target for all except MinGW
platform_build_seal: $(GENERATED_SOURCES)
	$(MAKE) -C $(BUILDDIR) $(TARGET)
	$(MakeDir) $(OUTDIR)
	$(Copy) build/$(OUTDIR)/$(SEAL_EXECUTABLE) $(OUTDIR)/$(EXECUTABLE)

#endif

platform_skip:
	$(Print) "Skipping $(PROJDIR)/$(APPMOD) for $(TARGET): not for this platform!"
	$(Print) ""

ifeq ("$(SEAL_SOURCES)", "")
PLATFORM_UPLOAD_ACTION = platform_upload_normal
else
PLATFORM_UPLOAD_ACTION = platform_upload_seal
endif

platform_upload_normal: platform-upload

platform_upload_seal: $(UPLOAD_TARGET)

upload: $(PLATFORM_UPLOAD_ACTION)
	$(_QUIET) @echo ""

#
# Dispatch the correct platfor_xxx target depending on PLATFORM_ACTION variable
#
$(PLATFORM_TARGET): $(PLATFORM_ACTION)

#
# Generate clean<platform> targets for each platform
#
$(PLATFORMS:%=clean%): clean

#
# Save platform name to a makefile to enable later reuse
#
saveplatform:
	$(_QUIET) rm -f $(BUILDDIR)/Makefile.target
	$(_QUIET) @echo "saving Makefile.target"
	$(_QUIET) @echo >$(BUILDDIR)/Makefile.target "TARGET = $(TARGET)"

build: objDirs $(OUTDIR)/$(EXECUTABLE) stackdump-build

#
# "config" file is required for building. An empty file is created it doesn't exist
#
$(PROJDIR)/config:
	touch $(PROJDIR)/config

#
# Detect whether config file has changed, and rebuild all it has.
#
$(BUILDDIR)/config.saved: $(PROJDIR)/config
	$(_QUIET) $(Delete) $(BUILDDIR)
	$(_QUIET) mkdir -p $(BUILDDIR)
	$(_QUIET) cp $(PROJDIR)/config $(BUILDDIR)/config.saved

#
# Remove output files
#
clean: 
	$(Print) "RM $(OUTDIR)"
	$(_QUIET) $(Delete) $(OUTDIR) *.stackdump extflash.dat eeprom

# ===== header file dependency tracking =====

_DEPS := $(subst .res,.d,$(subst .o,.d,$(OBJS)))

ifneq ("", "$(filter-out clean cleanall, $(MAKECMDGOALS))")
-include $(_DEPS)
endif

#===== Rules =====

# .c -> .o, system sources
$(OUTDIR)/%.o : $(OSW)/%.c
	$(Print) "CC $<"
	$(_QUIET) $(CC) $(CFLAGS) $(INCLUDES) -MD -MF $(subst .o,.d,$@) -MP -MT $(subst .o,.d,$@) -MT $@ -c $< -o $@
ifeq ($(USE_RAM_EXECUTION),y)
# rename ".text" section to ".ramtext"
	$(_QUIET) $(OBJCOPY) --rename-section .text=.ramtext $@
endif

# .c -> .o, user sources
# objDirs was added as dependency because otherwise on MinGW they are not built.
$(OUTDIR)/%.o : $(PROJDIR)/%.c objDirs
#	echo MAKECMDGOALS=$(MAKECMDGOALS)
	$(Print) "CC $<"
	$(_QUIET) $(CC) $(CFLAGS) $(INCLUDES) -MD -MF $(subst .o,.d,$@) -MP -MT $(subst .o,.d,$@) -MT $@ -c $< -o $@
ifeq ($(USE_REPROGRAMMING),y)
# rename ".text" section to ".usertext"
	$(_QUIET) $(OBJCOPY) --rename-section .text=.usertext $@
endif
ifeq ($(USE_RAM_EXECUTION),y)
# rename ".text" section to ".ramtext"
	$(_QUIET) $(OBJCOPY) --rename-section .text=.ramtext $@
endif

# .sl -> .c, SEAL sources
$(BUILDDIR)/%.c : %.sl
	$(Print) "SEAL $<"
	$(_QUIET) $(SEAL) --arch $(PLATFORM) -o $@ $<

# .o -> .elf
$(OUTDIR)/$(APPMOD).elf : $(OBJS)
	$(Print) "LD $@"
	$(_QUIET) $(LINKER_SCRIPT) $(ARCH) $@ '$(APP_OBJS)' '$(POBJS)' '$(CFLAGS) $(LDFLAGS)'

# .o -> .exe
$(OUTDIR)/$(APPMOD).exe : $(OBJS)
	$(Print) "LD $@"
	$(_QUIET) $(LINKER_SCRIPT) 'pc' $@ '$(APP_OBJS)' '$(POBJS)' '$(CFLAGS) $(LDFLAGS)'

# .c -> .rel
$(OUTDIR)/%.rel : $(OSW)/%.c 
	$(Print) "CC $<"
	$(_QUIET) $(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(OUTDIR)/%.rel : $(PROJDIR)/%.c 
	$(Print) "CC $<"
	$(_QUIET) $(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# .rel -> .ihx (nrf)
$(OUTDIR)/$(APPMOD).ihx : $(subst .o,.rel,$(OBJS))
	$(Print) "LD $@"
	$(_QUIET) $(LINKER) $^ "$(LDFLAGS)" -o $@


# Create target directory
$(OUTDIR):
	$(_QUIET) $(MakeDir) $@

objDirs:
	$(_QUIET) $(MakeDir) $(OBJDIRS)

motelist:
	$(_QUIET) $(MOTELIST)

serial:
	$(_QUIET) $(SERIAL) -s $(BSLPORT) -b $(BAUDRATE) -p $(PLATFORM)

listen:
	$(_QUIET) $(SERIAL) -s $(BSLPORT) -b $(BAUDRATE) -p $(PLATFORM)

sensorlist:
	$(_QUIET) $(SENSORLIST) $(PLATFORM) "$(CFLAGS) $(LDFLAGS)"
