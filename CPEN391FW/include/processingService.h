/**
 * This module contains function declarations for processingService.c
 */

#ifndef PROCESSINGSERVICE_H_
#define PROCESSINGSERVICE_H_

void generateKey(unsigned wifi_lat, unsigned wifi_long, unsigned char key[]);
void regenerateKey(int gen_num, unsigned wifi_lat, unsigned wifi_long, unsigned char key[]);
int upload(char *fileId, int packetNumber, int totalPackets, char *fileData);

#endif /* PROCESSINGSERVICE_H_ */
