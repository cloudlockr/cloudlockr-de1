/**
 * This module contains function declarations for bluetoothService.c
 */

#ifndef BLUETOOTHSERVICE_H_
#define BLUETOOTHSERVICE_H_

char *bluetooth_wait_for_data(void);
void bluetooth_send_message(char *data);
void bluetooth_send_status(int status);
char *bluetooth_wait_for_data(void);

#endif /* BLUETOOTHSERVICE_H_ */
