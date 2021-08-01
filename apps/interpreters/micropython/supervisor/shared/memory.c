/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "supervisor/memory.h"

#include <stddef.h>

#include "supervisor/shared/display.h"

#define CIRCUITPY_SUPERVISOR_ALLOC_COUNT 8

static supervisor_allocation allocations[CIRCUITPY_SUPERVISOR_ALLOC_COUNT];

void memory_init(void) {

}

void free_memory(supervisor_allocation* allocation) {
    int32_t index = 0;
    bool found = false;
    for (index = 0; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index++) {
        found = allocation == &allocations[index];
        if (found) {
            break;
        }
    }
    if (!found) {
        // Bad!
        // TODO(tannewt): Add a way to escape into safe mode on error.
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    free(allocation->ptr);
#pragma GCC diagnostic pop
    allocation->ptr = NULL;
}

supervisor_allocation* allocate_remaining_memory(void) {
    return allocate_memory(0x8000, false);
}

supervisor_allocation* allocate_memory(uint32_t length, bool high) {
    uint8_t index = 0;
    int8_t direction = 1;
    if (high) {
        index = CIRCUITPY_SUPERVISOR_ALLOC_COUNT - 1;
        direction = -1;
    }
    for (; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index += direction) {
        if (allocations[index].ptr == NULL) {
            break;
        }
    }
    if (index >= CIRCUITPY_SUPERVISOR_ALLOC_COUNT) {
        return NULL;
    }
    supervisor_allocation* alloc = &allocations[index];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    alloc->ptr = (uint32_t* )malloc(length);
#pragma GCC diagnostic pop
    alloc->length = length;
    return alloc;
}

void supervisor_move_memory(void) {
    //supervisor_display_move_memory();
}
