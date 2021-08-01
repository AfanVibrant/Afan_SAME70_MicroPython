# Standard includes

-include $(TOPDIR)/.config

ifeq ($(CONFIG_ARCH_CHIP_SAMV7),y)
CIRCUITPY_ARCH_BOARD = samv71-xult
endif