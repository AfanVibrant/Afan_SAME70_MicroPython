#include <unistd.h>
#include <stdio.h>
#include "py/mpconfig.h"

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    return (getc(stdin));
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
	if ((str != NULL) && (len > 0)) {
		for (mp_uint_t tx_num = 0; tx_num < len; tx_num++) {
			putc(str[tx_num],stdout);
		}
	}
}
