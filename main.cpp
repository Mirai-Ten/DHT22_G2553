/**
 ** Main.
 **
 ** Based on Eugene Mamin project
 **/

#include  <stdio.h>

#include "definitions.h"
#include "dht_lib.h"
#include "work_cycle.h"

DHT dht22;

char sensorsdatabuf[STR_BUF_SZ];
volatile char speedbuf;

volatile uint16_t last_succ_speed = 0;

volatile bool has_new_rx_data_uart = false;
volatile bool has_new_dht_data = false;

uint16_t hint, hdec, tint, tdec;

// Read sensor data and send.
void handle_dht22 ();
// Handle received control data.
void handle_UART ();

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;

    wcycle_init ();

    __bis_SR_register (GIE);

    wcycle_dht_read ();

    while (true) {
        handle_dht22 ();
        handle_UART ();
	}
}

void handle_dht22 () {
	if (has_new_dht_data) {
		has_new_dht_data = false;

		int dht22_status = dht22.decode();
		if (dht22_status == DHT_SUCCESS_STATUS) {
			dht22.humidity(&hint, &hdec);
			dht22.temperature(&tint, &tdec);
			sprintf (sensorsdatabuf, "H:(%d.%d)T:(%d.%d)S:(%d)^|", hint, hdec, tint, tdec, last_succ_speed);
		} else {
			sprintf (sensorsdatabuf, "[DHT22] I/O error:%d^|", dht22_status);
		}

		wcycle_send (sensorsdatabuf);
		wcycle_dht_read ();
	}
}

void handle_UART () {
	if (has_new_rx_data_uart) {

		has_new_rx_data_uart = false;

		P1OUT ^= RED_LED;
	}
}

