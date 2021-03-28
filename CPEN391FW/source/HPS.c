/*
 * HSP.h
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * Board Support Package (BSP) of Cyclone V SoC HPS.
 */


 
#include "TypeDef.h"
#include "HPS.h"


/*------------------- Constants Define ---------------------------------------*/
/*------------------- Type Define --------------------------------------------*/
/*------------------- Local Data ---------------------------------------------*/
/*------------------- Local Function Prototype -------------------------------*/
/*------------------- Local Function -----------------------------------------*/
/*------------------- API Function -------------------------------------------*/

/**************************************************************************
** Sleep for amount of time given by time interval time_us in us.
**
** Parameters
** time_us - Time interval in us.
**
***************************************************************************/
void HPS_usleep( unsigned int time_us )
{
    int data;
    unsigned int i, j;
    
    for ( i = 0; i < time_us; i++ )
    {
    	// CPU 200 MHz
    	for ( j = 0; j < 200; j++ )
    	{
    		data >> 1;
    	}
    }
}
