/**
 * This module contains function declarations for wifiService.c
 */

#ifndef WIFI_H_
#define WIFI_H_
int set_wifi_config(char *network_name, char *network_password);
int get_file_metadata(char *file_id);
int upload_data(char *file_id, int blob_number, char *file_data);
char *get_blob(char *file_id, int blob_number);
#endif // WIFI_H_
