/**
 * This module contains function declarations for hpsService.c
 */

#ifndef HPSSERVICE_H_
#define HPSSERVICE_H_

extern volatile uint32 *PtimerCount;

void hps_init(void);
void hps_process(void);
bool hps_elapsed_us(uint32 start, uint32 TimeUs);
void hps_us_delay(unsigned int time_us);
void hps_ms_delay(unsigned int time_ms);
void hps_toggle_ledg(void);
void hps_usleep(unsigned int time_us);

#endif /* HPSSERVICE_H_ */
