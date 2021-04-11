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
int set_wifi_config(char *network_name, char *network_password);
char *getFileMetadata(char *fileId);
char *uploadData(char *email, char *fileId, char *packetNumber, char *totalPackets, char *fileData);
#endif // WIFI_H_
