/**
 * This module contains function declarations for processingService.c
 */

#ifndef PROCESSINGSERVICE_H_
#define PROCESSINGSERVICE_H_

void generate_key(unsigned wifi_lat, unsigned wifi_long, unsigned char key[]);
void regenerate_key(char *encryption_component, unsigned wifi_lat, unsigned wifi_long, unsigned char key[]);
char *upload(char *file_id, int packet_number, int total_packets, char *file_data);
void download(char *file_id, char *encryption_component);

#endif /* PROCESSINGSERVICE_H_ */
