-include $(TOPDIR)/.config

# NuttX board specific
include nxboard.mk

# Port-specific
include boards/$(CIRCUITPY_ARCH_BOARD)/mpconfigport.mk

SRC_SUPERVISOR = \
	boards/supervisor/port.c \
	supervisor/shared/autoreload.c \
	supervisor/shared/display.c \
	supervisor/shared/flash.c \
	supervisor/shared/micropython.c \
	supervisor/shared/rgb_led_status.c \
	supervisor/shared/safe_mode.c \
	supervisor/shared/stack.c \
	supervisor/shared/status_leds.c \
	supervisor/shared/translate.c \
	supervisor/shared/memory.c \
	supervisor/stub/serial.c \
	supervisor/stub/filesystem.c

ifdef $(CIRCUITPY_VFS_ENABLE)
SRC_SUPERVISOR += supervisor/shared/filesystem.c
endif

ifeq ($(CONFIG_MICROPYTHON_SUPPORT_WINC1500),y)
SRC_SUPERVISOR += \
	supervisor/shared/winc1500/winc1500.c \
	supervisor/shared/winc1500/winc1500_ap.c \
	supervisor/shared/winc1500/winc1500_sta.c
endif

ifndef $(NO_USB)
NO_USB = $(wildcard boards/supervisor/usb.c)
endif

ifneq ($(INTERNAL_FLASH_FILESYSTEM),)
	CFLAGS += -DINTERNAL_FLASH_FILESYSTEM=$(INTERNAL_FLASH_FILESYSTEM)
endif
ifneq ($(QSPI_FLASH_FILESYSTEM),)
# EXPRESS_BOARD is obsolete and should be removed when samd-peripherals is updated.
	CFLAGS += -DQSPI_FLASH_FILESYSTEM=$(QSPI_FLASH_FILESYSTEM) -DEXPRESS_BOARD
endif
ifneq ($(SPI_FLASH_FILESYSTEM),)
# EXPRESS_BOARD is obsolete and should be removed when samd-peripherals is updated.
	CFLAGS += -DSPI_FLASH_FILESYSTEM=$(SPI_FLASH_FILESYSTEM) -DEXPRESS_BOARD
endif

# Choose which flash filesystem impl to use.
# (Right now INTERNAL_FLASH_FILESYSTEM and SPI_FLASH_FILESYSTEM are mutually exclusive.
# But that might not be true in the future.)
ifdef EXTERNAL_FLASH_DEVICES
	CFLAGS += -DEXTERNAL_FLASH_DEVICES=$(EXTERNAL_FLASH_DEVICES) \
				-DEXTERNAL_FLASH_DEVICE_COUNT=$(EXTERNAL_FLASH_DEVICE_COUNT)

	SRC_SUPERVISOR += supervisor/shared/external_flash/external_flash.c
	ifeq ($(SPI_FLASH_FILESYSTEM),1)
		SRC_SUPERVISOR += supervisor/shared/external_flash/spi_flash.c
	else
	endif
	ifeq ($(QSPI_FLASH_FILESYSTEM),1)
		SRC_SUPERVISOR += boards/supervisor/qspi_flash.c supervisor/shared/external_flash/qspi_flash.c
	endif
else
	SRC_SUPERVISOR += boards/supervisor/internal_flash.c
endif

ifeq ($(USB),FALSE)
	ifeq ($(wildcard supervisor/serial.c),)
		SRC_SUPERVISOR += supervisor/stub/serial.c
	else
		SRC_SUPERVISOR += supervisor/serial.c
	endif
else
#	SRC_SUPERVISOR += supervisor/shared/serial.c
	CFLAGS += -DUSB_AVAILABLE
endif

#SUPERVISOR_O = $(addprefix $(BUILD)/, $(SRC_SUPERVISOR:.c=.o)) $(BUILD)/autogen_display_resources.o
SUPERVISOR_O = $(addprefix $(BUILD)/, $(SRC_SUPERVISOR:.c=.o))

$(BUILD)/supervisor/shared/translate.o: $(HEADER_BUILD)/qstrdefs.generated.h

#$(BUILD)/autogen_usb_descriptor.c $(BUILD)/genhdr/autogen_usb_descriptor.h: autogen_usb_descriptor.intermediate

#.INTERMEDIATE: autogen_usb_descriptor.intermediate

#autogen_usb_descriptor.intermediate: micropython-1.9.4/tools/gen_usb_descriptor.py Makefile | $(HEADER_BUILD)
#	$(STEPECHO) "GEN $@"
#	$(Q)install -d $(BUILD)/genhdr
#	$(Q)$(PYTHON3) micropython-1.9.4/tools/gen_usb_descriptor.py \
#		--manufacturer $(USB_MANUFACTURER)\
#		--product $(USB_PRODUCT)\
#		--vid $(USB_VID)\
#		--pid $(USB_PID)\
#		--serial_number_length $(USB_SERIAL_NUMBER_LENGTH)\
#		--output_c_file $(BUILD)/autogen_usb_descriptor.c\
#		--output_h_file $(BUILD)/genhdr/autogen_usb_descriptor.h

#CIRCUITPY_DISPLAY_FONT ?= "micropython-1.9.4/tools/fonts/ter-u12n.bdf"

#$(BUILD)/autogen_display_resources.c: micropython-1.9.4/tools/gen_display_resources.py $(HEADER_BUILD)/qstrdefs.generated.h Makefile | $(HEADER_BUILD)
#	$(STEPECHO) "GEN $@"
#	$(Q)install -d $(BUILD)/genhdr
#	$(Q)$(PYTHON3) micropython-1.9.4/tools/gen_display_resources.py \
#		--font $(CIRCUITPY_DISPLAY_FONT) \
#		--sample_file $(HEADER_BUILD)/qstrdefs.generated.h \
#		--output_c_file $(BUILD)/autogen_display_resources.c
