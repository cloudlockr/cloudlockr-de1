/**
 * This module contains function declarations for processingService.c
 */

#ifndef PROCESSINGSERVICE_H_
#define PROCESSINGSERVICE_H_

void generate_key(char *location, unsigned char key[]);
void regenerate_key(char *encryption_component, char *location, unsigned char key[]);
char *upload(char *file_id, int packet_number, int total_packets, char *location, char *file_data);
void download(char *file_id, char *encryption_component, char *location);

#endif /* PROCESSINGSERVICE_H_ */
