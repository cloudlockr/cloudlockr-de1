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
int get_file_metadata(char *file_id);
int upload_data(char *file_id, int blob_number, char *file_data);
char *get_blob(char *file_id, int blob_number);
#endif // WIFI_H_
