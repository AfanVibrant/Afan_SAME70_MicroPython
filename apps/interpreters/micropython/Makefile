############################################################################
# apps/ interpreters/micropython/Makefile
#
#   Copyright (C) 2015, 2016 Gregory Nutt. All rights reserved.
#   Authors: Gregory Nutt <gnutt@nuttx.org>
#            Dave Marples <dave@marples.net>
#            Kevin Liu    <lj2057@163.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name NuttX nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
###########################################################################

# Standard includes

-include $(TOPDIR)/.config
-include $(TOPDIR)/Make.defs
include $(APPDIR)/Make.defs

# Set up build configuration and environment

WD := ${shell pwd | sed -e 's/ /\\ /g'}

CONFIG_INTERPRETERS_MICROPYTHON_URL ?= "https://github.com/micropython/micropython/archive"
CONFIG_INTERPRETERS_MICROPYTHON_VERSION ?= "1.12"
MICROPYTHON_VERSION = $(patsubst "%",%,$(strip $(CONFIG_INTERPRETERS_MICROPYTHON_VERSION)))

MICROPYTHON_TARBALL = v$(MICROPYTHON_VERSION).tar.gz
WGET ?= wget

MICROPYTHON_UNPACKNAME = micropython-$(MICROPYTHON_VERSION)
UNPACK ?= tar -zxf

MICROPYTHON_UNPACKDIR =  $(WD)/$(MICROPYTHON_UNPACKNAME)
MICROPYTHON_SRCDIR = $(MICROPYTHON_UNPACKDIR)/py

# Get the Micro Python build environment.  mkenv.mk has these dependencies
#
#  V             - Enable verbose output (same meaning as in NuttX)
#  CROSS_COMPILE - Tool prefix (same as CROSSDEV in NuttX)

CROSS_COMPILE ?= $(CROSSDEV)

include $(MICROPYTHON_SRCDIR)/mkenv.mk

# qstr definitions (must come before including py.mk)

QSTR_DEFS = qstrdefsport.h

# Include py core make definitions

include $(MICROPYTHON_SRCDIR)/py.mk

# NuttX board specific
include nxboard.mk

# Port-specific
MICROPY_FLOAT_IMPL = double

#include supervisor/supervisor.mk

# c99 is needed for Micro Python

CFLAGS += -std=c99 -Wno-shadow -Wno-strict-prototypes

INC =  -I. -I$(WD) -I$(MICROPYTHON_UNPACKDIR) -I$(PY_SRC) -I$(BUILD)

CFLAGS +=$(INC)
CFLAGS += -DFFCONF_H=\"ffconf.h\"

ifeq ($(CONFIG_WINDOWS_NATIVE),y)
  ARCH_SRCDIR = $(TOPDIR)\arch\$(CONFIG_ARCH)\src
  ARCH_INCDIR = $(TOPDIR)\arch\$(CONFIG_ARCH)\include\$(CONFIG_ARCH_CHIP)
  NUTTX = "$(TOPDIR)\nuttx$(EXEEXT)"
  CFLAGS += -I$(ARCH_SRCDIR)\chip
  CFLAGS += -I$(ARCH_SRCDIR)\common
  CFLAGS += -I$(ARCH_SRCDIR)\$(ARCH_SUBDIR)
  CFLAGS += -I$(TOPDIR)\sched
else
  ARCH_SRCDIR = $(TOPDIR)/arch/$(CONFIG_ARCH)/src
  ARCH_INCDIR = $(TOPDIR)/arch/$(CONFIG_ARCH)/include/$(CONFIG_ARCH_CHIP)
ifeq ($(WINTOOL),y)
  NUTTX = "${shell cygpath -w $(TOPDIR)/nuttx$(EXEEXT)}"
  CFLAGS += -I "${shell cygpath -w $(ARCH_SRCDIR)/chip}"
  CFLAGS += -I "${shell cygpath -w $(ARCH_SRCDIR)/common}"
  CFLAGS += -I "${shell cygpath -w $(ARCH_SRCDIR)/$(ARCH_SUBDIR)}"
  CFLAGS += -I "${shell cygpath -w $(TOPDIR)/sched}"
else
  NUTTX = "$(TOPDIR)/nuttx$(EXEEXT)"
  CFLAGS += -I$(ARCH_SRCDIR)/chip
  CFLAGS += -I$(ARCH_SRCDIR)/common
  CFLAGS += -I$(ARCH_SRCDIR)/$(ARCH_SUBDIR)
  CFLAGS += -I$(ARCH_SRCDIR)/$(CONFIG_ARCH_FAMILY)
  CFLAGS += -I$(TOPDIR)/sched
  CFLAGS += -I$(TOPDIR)/include
  CFLAGS += -I$(ARCH_INCDIR)
  CFLAGS += -Iboards/$(MICROPY_BOARD_PORT)
endif
endif

ifeq ($(MICROPY_FLOAT_IMPL),double)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE
else
ifeq ($(MICROPY_FLOAT_IMPL),single)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
endif
endif

# Built-in application info

CONFIG_INTERPRETERS_MICROPYTHON_APPNAME ?= micropython
CONFIG_INTERPRETERS_MICROPYTHON_STACKSIZE ?= 16384
CONFIG_INTERPRETERS_MICROPYTHON_PRIORITY ?= SCHED_PRIORITY_DEFAULT

APPNAME = $(CONFIG_INTERPRETERS_MICROPYTHON_APPNAME)
STACKSIZE = $(CONFIG_INTERPRETERS_MICROPYTHON_STACKSIZE)
PRIORITY = $(CONFIG_INTERPRETERS_MICROPYTHON_PRIORITY)

CONFIG_INTERPRETERS_MICROPYTHON_PROGNAME ?= micropython$(EXEEXT)
PROGNAME = $(CONFIG_INTERPRETERS_MICROPYTHON_PROGNAME)

# Include make rules and variables common across CircuitPython builds.

# Files
#lib/libc/string0.c \

ASRCS = boards/gchelper.s

CSRCS = \
	nx_stdout.c \
	nx_initial.c \
	nx_execfile.c \
	nx_readline.c \
	nx_pyexec.c \
	nx_gccollect.c \
	nuttxpy_threadport.c \
	lib/utils/printf.c \
	lib/utils/buffer_helper.c \
	lib/utils/stdout_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/context_manager_helpers.c \
	$(BUILD)/_frozen_mpy.c \

# CIRCUITPY_SAMD is handled in the atmel-samd tree.
# Only for SAMD chips.
# Assume not a SAMD build.
CFLAGS += -DCIRCUITPY_SAMD=$(CIRCUITPY_SAMD)

# Currently always off.
CFLAGS += -DCIRCUITPY_STAGE=$(CIRCUITPY_STAGE)

CFLAGS += -DCIRCUITPY_STORAGE=$(CIRCUITPY_STORAGE)
CFLAGS += -DCIRCUITPY_STRUCT=$(CIRCUITPY_STRUCT)
CFLAGS += -DCIRCUITPY_SUPERVISOR=$(CIRCUITPY_SUPERVISOR)
CFLAGS += -DCIRCUITPY_TIME=$(CIRCUITPY_TIME)
CFLAGS += -DCIRCUITPY_TOUCHIO=$(CIRCUITPY_TOUCHIO)

# For debugging.
CFLAGS += -DCIRCUITPY_UHEAP=$(CIRCUITPY_UHEAP)

CFLAGS += -DCIRCUITPY_USB_HID=$(CIRCUITPY_USB_HID)
CFLAGS += -DCIRCUITPY_USB_MIDI=$(CIRCUITPY_USB_MIDI)
CFLAGS += -DCIRCUITPY_PEW=$(CIRCUITPY_PEW)
CFLAGS += -DCIRCUITPY_WIFIIO=$(CIRCUITPY_WIFIIO)

# For debugging.
CFLAGS += -DCIRCUITPY_USTACK=$(CIRCUITPY_USTACK)

###
# Select which builtin modules to compile and include.

ifeq ($(MICROPY_FLOAT_IMPL),double)
SRC_LIBM = $(addprefix lib/libm_dbl/,\
	__cos.c \
	__expo2.c \
	__fpclassify.c \
	__rem_pio2.c \
	__rem_pio2_large.c \
	__signbit.c \
	__sin.c \
	__tan.c \
	acos.c \
	acosh.c \
	asin.c \
	asinh.c \
	atan.c \
	atan2.c \
	atanh.c \
	ceil.c \
	cos.c \
	cosh.c \
	copysign.c \
	erf.c \
	exp.c \
	expm1.c \
	floor.c \
	fmod.c \
	frexp.c \
	ldexp.c \
	lgamma.c \
	log.c \
	log10.c \
	log1p.c \
	modf.c \
	nearbyint.c \
	pow.c \
	rint.c \
	scalbn.c \
	sin.c \
	sinh.c \
	tan.c \
	tanh.c \
	tgamma.c \
	trunc.c \
	)
ifeq ($(SUPPORTS_HARDWARE_FP_DOUBLE),1)
SRC_LIBM += lib/libm_dbl/thumb_vfp_sqrt.c
else
SRC_LIBM += lib/libm_dbl/sqrt.c
endif
else
ifeq ($(MICROPY_FLOAT_IMPL),single)
SRC_LIBM = $(addprefix lib/libm/,\
	math.c \
	thumb_vfp_sqrtf.c \
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	nearbyintf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
	)
endif
endif

ifneq ($(CONFIG_BUILD_KERNEL),y)
MAINSRC = micropython_main.c
endif

# List of sources for qstr extraction
SRC_QSTR += $(MAINSRC)
SRC_QSTR += $(SRC_C)

# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=


$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(MICROPYTHON_UNPACKDIR)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(CSRCS:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(ASRCS:.s=.o))

ifeq ($(MICROPY_FLOAT_IMPL),double)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
else
ifeq ($(MICROPY_FLOAT_IMPL),single)
#OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
endif

ifneq ($(CONFIG_BUILD_KERNEL),y)
  OBJ += $(addprefix $(BUILD)/, $(MAINSRC:.c=$(OBJEXT)))
endif

ifeq ($(CONFIG_WINDOWS_NATIVE),y)
  BIN = ..\..\libapps$(LIBEXT)
else
ifeq ($(WINTOOL),y)
  BIN = ..\\..\\libapps$(LIBEXT)
else
  BIN = ../../libapps$(LIBEXT)
endif
endif

ifeq ($(WINTOOL),y)
  INSTALL_DIR = "${shell cygpath -w $(BIN_DIR)}"
else
  INSTALL_DIR = $(BIN_DIR)
endif

ROOTDEPPATH = --dep-path .

# Common build

all: .built
.PHONY: install context clean distclean preconfig
.PRECIOUS: ../../libapps$(LIBEXT)

$(MICROPYTHON_TARBALL):
	@echo "Downloading: $(MICROPYTHON_TARBALL)"
	$(Q) $(WGET) $(CONFIG_INTERPRETERS_MICROPYTHON_URL)/$(MICROPYTHON_TARBALL)

$(MICROPYTHON_UNPACKNAME): $(MICROPYTHON_TARBALL)
	@echo "Unpacking: $(MICROPYTHON_TARBALL) -> $(MICROPYTHON_UNPACKNAME)"
	$(Q) $(UNPACK) $(MICROPYTHON_TARBALL)
	@touch $(MICROPYTHON_UNPACKNAME)

.built: $(MICROPYTHON_UNPACKNAME) $(OBJ)
	$(call ARCHIVE, -r $(BIN), $(OBJ))
	@touch $@

ifeq ($(CONFIG_BUILD_KERNEL),y)
$(BIN_DIR)$(DELIM)$(PROGNAME): $(MICROPYTHON_UNPACKNAME) $(OBJ) $(MAINOBJ)
	@echo "LD: $(PROGNAME)"
	$(Q) $(LD) $(LDELFFLAGS) $(LDLIBPATH) -o $(INSTALL_DIR)$(DELIM)$(PROGNAME) $(ARCHCRT0OBJ) $(MAINOBJ) $(LDLIBS)
	$(Q) $(NM) -u $(INSTALL_DIR)$(DELIM)$(PROGNAME)

install: $(BIN_DIR)$(DELIM)$(PROGNAME)
else
install:
endif

ifeq ($(CONFIG_NSH_BUILTIN_APPS),y)
$(BUILTIN_REGISTRY)$(DELIM)$(APPNAME)_main.bdat: $(DEPCONFIG) Makefile
	$(call REGISTER,$(APPNAME),$(PRIORITY),$(STACKSIZE),micropython_main)

context: $(MICROPYTHON_UNPACKNAME) $(BUILTIN_REGISTRY)$(DELIM)$(APPNAME)_main.bdat
else
context:
endif

.depend: Makefile $(SRCS)
	@$(MKDEP) $(ROOTDEPPATH) "$(CC)" -- $(CFLAGS) -- $(SRCS) >Make.dep
	@touch $@

depend: .depend

clean:
	$(call DELDIR, build)
	$(call DELFILE, .built)
	$(call CLEAN)

distclean: clean
	$(call DELFILE, Make.dep)
	$(call DELFILE, .depend)
	$(call DELDIR, $(MICROPYTHON_UNPACKNAME))
	$(call DELFILE, .downloaded)
	$(call DELFILE, $(MICROPYTHON_TARBALL))

preconfig:

-include $(MICROPYTHON_SRCDIR)/mkrules.mk
-include Make.dep
