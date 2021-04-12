/*
 * HPS.h
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * Board Support Package (BSP) of Cyclone V SoC HPS.
 */


/*------------------- Constants Define ---------------------------------------*/ 

#define GPIO1_DR    (volatile uint32*)(0xFF709000)
#define GPIO1_DDR   (volatile uint32*)(0xFF709004)


/*------------------- Type Define --------------------------------------------*/

/*------------------- Global Data --------------------------------------------*/
extern volatile uint32 *PtimerCount;

/*------------------- API Function -------------------------------------------*/

// Exported functions
void HPS_Init( void );
void HPS_Process( void );
bool HPS_ElapsedUs( uint32 start, uint32 TimeUs );
void HPS_usDelay( unsigned int time_us );
void HPS_msDelay( unsigned int time_ms );
void HPS_ToggleLedG( void );
void HPS_usleep( unsigned int time_us );


