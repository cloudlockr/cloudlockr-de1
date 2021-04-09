/*
 * WIFI.h
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * This module implement wifi related functionalities.
 */

#ifndef WIFI_H_
#define WIFI_H_
#include <stdbool.h>


char* uploadData(char* email, char* fileId, char* packetNumber, char* totalPackets, char* fileData);
char* get_wifi_networks( void ); // Look at AT+CWLAP
//int set_wifi_config(char* networkName, char* networkPassword);
char* getFileMetadata(char* fileId);
char* getBlob(char* fileId, char* blobNumber);

bool esp8266_send_command(const char *cmd);
bool esp8266_send_data( const char *data, int length, char *buffer);



#endif /* WIFI_H_ */
