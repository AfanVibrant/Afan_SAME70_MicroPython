/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Configuration options for the peripherals library. Copy into your project and edit
// it as needed. Ensure it's in an include path because the library does:
// #include "samd_peripherals_config.h"

#ifndef MICROPY_INCLUDED_HAL_MPCONFIG_H
#define MICROPY_INCLUDED_HAL_MPCONFIG_H

#include "board_mpconfig.h"

#define BOARD_I2C  (defined(DEFAULT_I2C_BUS_SDA) && defined(DEFAULT_I2C_BUS_SCL))
#define BOARD_SPI  (defined(DEFAULT_SPI_BUS_SCK) && defined(DEFAULT_SPI_BUS_MISO) && defined(DEFAULT_SPI_BUS_MOSI))
#define BOARD_UART (defined(DEFAULT_UART_BUS_RX) && defined(DEFAULT_UART_BUS_TX))

// Define certain native modules with weak links so they can be replaced with Python
// implementations. This list may grow over time.
#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \

// Native modules that are weak links can be accessed directly
// by prepending their name with an underscore. This list should correspond to
// MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS, assuming you want the native modules
// to be accessible when overriden.
#define MICROPY_PORT_BUILTIN_MODULE_ALT_NAMES \

// This is an inclusive list that should correspond to the CIRCUITPY_XXX list above,
// including dependencies such as TERMINALIO depending on DISPLAYIO (shown by indentation).
// Some of these definitions will be blank depending on what is turned on and off.
// Some are omitted because they're in MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS above.
#define MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS \

// If weak links are enabled, just include strong links in the main list of modules,
// and also include the underscore alternate names.
#if MICROPY_MODULE_WEAK_LINKS
#define MICROPY_BOARD_BUILTIN_MODULES \
    MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS \
    MICROPY_PORT_BUILTIN_MODULE_ALT_NAMES
#else
// If weak links are disabled, included both strong and potentially weak lines
#define MICROPY_BOARD_BUILTIN_MODULES \
    MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS \
    MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif

#endif // MICROPY_INCLUDED_HAL_MPCONFIG_H
