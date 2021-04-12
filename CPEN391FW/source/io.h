#ifndef __IO_H__
#define __IO_H__

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2009 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/

/* IO Header file for MIPS32 Toolchain */

#include "alt_types.h"
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef SYSTEM_BUS_WIDTH
#define SYSTEM_BUS_WIDTH 32
#endif

/* 
 * Converts a KSEG0 virtual address (Kernel unmapped cached) to a
 * KSEG1 virtual address (Kernel unmapped uncached).
 * Used to ensure that a virtual address will be accessed uncached.
 */
#define __KSEG0_TO_KSEG1(ADDR) ((ADDR) | 0xa0000000)
#define UNCACHEABLE_ADDR(ADDR) (__KSEG0_TO_KSEG1(ADDR))

/* Dynamic bus access functions */

#define __IO_CALC_ADDRESS_DYNAMIC(BASE, OFFSET) \
  ((void*)(__KSEG0_TO_KSEG1((uint32)BASE) + (OFFSET)))

#define IORD_32DIRECT(BASE, OFFSET) \
  (*((volatile uint32*)(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))))
#define IORD_16DIRECT(BASE, OFFSET) \
  (*((volatile uint16*)(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))))
#define IORD_8DIRECT(BASE, OFFSET) \
  (*((volatile uint8*)(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))))

#define IOWR_32DIRECT(BASE, OFFSET, DATA) \
  (*((volatile uint32*)(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))) = (DATA))
#define IOWR_16DIRECT(BASE, OFFSET, DATA) \
  (*((volatile uint16*)(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))) = (DATA))
#define IOWR_8DIRECT(BASE, OFFSET, DATA) \
  (*((volatile uint8*)(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))) = (DATA))

/* Native bus access functions */

#define __IO_CALC_ADDRESS_NATIVE(BASE, REGNUM) \
  ((void*)(__KSEG0_TO_KSEG1((uint32)BASE) + ((REGNUM) * (SYSTEM_BUS_WIDTH/8))))

#define IORD(BASE, REGNUM) \
  (*((volatile uint32*)(__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM)))))
#define IOWR(BASE, REGNUM, DATA) \
  (*((volatile uint32*)(__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM)))) = (DATA))

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */
