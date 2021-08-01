# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Author: Kevin Liu    <lj2057@163.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

-include $(TOPDIR)/.config
-include $(TOPDIR)/Make.defs
include $(APPDIR)/Make.defs

# Include NuttX Python configuration

-include ../nuttxconfig.mk

# NuttX Python Board directory

BOARD_PATH ?= $(NUTTXPY_ARCH_BOARD)

BOARD_CSRCS  = $(NUTTXPY_ARCH_BOARD)/pins.c
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/processor.c

ifeq ($(CONFIG_MICROPY_PY_BOARD_DIGITALIO),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/common-hal/digitalio/__init__.c
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/common-hal/digitalio/digitalinout.c
endif

ifeq ($(CONFIG_MICROPY_PY_WIFI_WINC1500),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/common-hal/winc1500/winc1500.c
endif

ifeq ($(CONFIG_MICROPY_PY_BOARD_BUSIO_UART),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/board_serial.c
endif

ifeq ($(CONFIG_MICROPY_PY_BOARD_BUSIO_SPI),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/board_spi.c
endif

ifeq ($(CONFIG_MICROPY_PY_BOARD_BUSIO_I2C),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/board_i2c.c
endif

ifeq ($(CONFIG_MICROPY_PY_BOARD_RTC),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/board_rtc.c
endif

ifeq ($(CONFIG_MICROPY_PY_BOARD_ANALOGIO_DAC),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/board_dac.c
endif

ifeq ($(CONFIG_MICROPY_PY_BOARD_ANALOGIO_ADC),y)
BOARD_CSRCS += $(NUTTXPY_ARCH_BOARD)/board_adc.c
endif

CFLAGS +=$(INC)
