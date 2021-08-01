/****************************************************************************
 * interpreters/micropython/mp_initial.c
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Authors: Kevin Liu (lj2057@163.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <debug.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>

#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mphal.h"

#include "mpconfigport.h"

static const char fresh_boot_py[] = {
	"# boot.py -- run on boot-up\r\n"
	"# can run arbitrary Python, but best to keep it minimal\r\n"
	"\r\n"
	"import machine\r\n"
	"import pyb\r\n"
	"#pyb.main('main.py') # main script to run after this one\r\n"
	"#pyb.usb_mode('VCP+MSC') # act as a serial and a storage device\r\n"
	"#pyb.usb_mode('VCP+HID') # act as a serial device and a mouse\r\n"
};

static const char fresh_main_py[] = {
	"# main.py -- put your code here!\r\n"
	"l = [1,3,5,7,9]\r\n"
	"m = l\r\n"
	"m\r\n"
	"x = [4,6,3,2,9,19,16]\r\n"
	"x.sort()\r\n"
	"x\r\n"
	"\r\n"	
	"def callback_on_connect(userdata, flags, rc):\r\n"
	"	mqtt.subscribe('/home/bedroom/msgbox', 0)\r\n"
	"\r\n"	
	"def callback_on_message(userdata, message):\r\n"
	"	print(message)\r\n"
	"	mqtt.publish('/home/bedroom/air', 'air turn on')\r\n"
	"\r\n"		
	"from umachine import Pin\r\n"
	"import utime\r\n"
	"led0 = Pin(23,Pin.OUT,Pin.PULL_UP)\r\n"
	"from umachine import I2C\r\n"
	"i2c0 = I2C(0)\r\n"	
	"from winc1500 import WLAN\r\n"	
	"wlan = WLAN(STA_IF)\r\n"	
	"wlan.connect('KSLINK034608 X', 'huangliQIN', AUTH_WPA_PSK)\r\n"	
	"from winc1500 import MQTT\r\n"
	"mqtt = MQTT(MQTT_CLIENT)\r\n"
	"mqtt.username_pw_set('winc_mp_mqtt', '')\r\n"
	"mqtt.on_connect(callback_on_connect)\r\n"
	"mqtt.on_message(callback_on_message)\r\n"
	"mqtt.connect('iot.eclipse.org', 1883, 30)\r\n"	
	"count = 0\r\n"
	"scan_i2c = 0\r\n"
	"delay = 100\r\n"
	"while (count < 40):\r\n"
	"	print('LED Blink loop :', count)\r\n"
	"	mqtt.publish('/home/bedroom/lamp', 'led on')\r\n"
	"	mqtt.publish('/home/bedroom/speaker', 'music off')\r\n"	
	"	count = count + 1\r\n"
	"	scan_i2c = scan_i2c + 1\r\n"
	"	led0.value(1)\r\n"
	"	if (delay <= 1000) :\r\n"
	"		delay = delay + 20\r\n"
	"	else :\r\n"
	"		delay = delay + 200\r\n"
	"	if (scan_i2c == 10) :\r\n"
	"		scan_ret0 = i2c0.scan()\r\n"	
	"		print(scan_ret0)\r\n"
	"	elif (scan_i2c == 20) :\r\n"
	"		i2c0.readfrom_mem(0x57, 0, 4)\r\n"	
	"	elif (scan_i2c == 30) :\r\n"
	"		i2c0.readfrom_mem(0x5f, 0x9a, 6)\r\n"
	"	elif (scan_i2c == 40) :\r\n"
	"		scan_ret1 = i2c0.scan()\r\n"	
	"		print(scan_ret1)\r\n"
	"	utime.sleep_ms(delay)\r\n"	
	"	led0.value(0)\r\n"
	"	mqtt.publish('/home/bedroom/lamp', 'led off')\r\n"	
	"	mqtt.publish('/home/bedroom/speaker', 'play music')\r\n"	
	"	utime.sleep_ms(delay)\r\n"
	"	mqtt.publish('/smart_home/bedroom/window', 'close window')\r\n"	
	"	\r\n"
	
};

static const char fresh_pybcdc_inf[] = {
//	#include "genhdr/atmel_pybcdc_inf.h"
};

static const char fresh_readme_txt[] = {
	"This is a MicroPython board\r\n"
	"\r\n"
	"You can get started right away by writing your Python code in 'main.py'.\r\n"
	"\r\n"
	"For a serial prompt:\r\n"
	" - Windows: you need to go to 'Device manager', right click on the unknown device,\r\n"
	"   then update the driver software, using the 'pybcdc.inf' file found on this drive.\r\n"
	"   Then use a terminal program like Hyperterminal or putty.\r\n"
	" - Mac OS X: use the command: screen /dev/tty.usbmodem*\r\n"
	" - Linux: use the command: screen /dev/ttyACM0\r\n"
	"\r\n"
	"Please visit http://micropython.org/help/ for further help.\r\n"
};

MP_NOINLINE bool write_to_file(FAR const char* fpath, FAR const void* fsrc) {
	bool ret = true;
	FILE * fp = fopen(fpath,"w");
	if (fp == NULL) {
		ret = false;
		mp_hal_stdout_tx_str("failed to create ");
		mp_hal_stdout_tx_str(fpath);
		mp_hal_stdout_tx_str("\n");
	} else {
		fwrite(fsrc, strlen(fsrc) - 1, 1, fp);
		fflush(fp);
		fclose(fp);
	}
	
	return ret;
}

// avoid inlining to avoid stack usage within main()
MP_NOINLINE bool init_flash_fs(uint reset_mode) {
	/* check if flash-based smartfs was ready for use */
	int fd;
	bool result = true;
	
	sched_lock();
	fd = open("/micropython/main.py",O_RDONLY);
	if (fd < 0) {
		result = write_to_file("/micropython/main.py", fresh_main_py);
		if (result == false) {
			goto outs;
		}
		
		result = write_to_file("/micropython/pybcdc.inf", fresh_pybcdc_inf);
		if (result == false) {
			goto outs;
		}
		
		result = write_to_file("/micropython/README.txt", fresh_readme_txt);
		if (result == false) {
			goto outs;
		}
		
		result = write_to_file("/micropython/boot.py", fresh_boot_py);
		sleep(1);
	} else {
		close(fd);
	}

outs:
	sched_unlock();
	return result;
}
