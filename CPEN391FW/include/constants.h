/**
 * This module contains project wide constants
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Max character length for master password
#define MAX_PASSWORD_LENGTH 32

// Max size for filedata sent from bluetooth
// #define MAX_FILEDATA_SIZE 0x00100000 // One megabyte
#define MAX_FILEDATA_SIZE 0x0020 // 32 bytes

// Buffer size for bluetooth
#define BUFFER_SIZE 1024

// Number of times to attempt connecting to router
#define HANDSHAKE 5

#endif /* CONSTANTS_H_ */
