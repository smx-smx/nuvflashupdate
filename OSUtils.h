/*
 * Copyright © 2010, Nuvoton Technology Corporation. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Nuvoton Technology Corporation. or the names of 
 * contributors or licensors may be used to endorse or promote products derived 
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind. 
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, 
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * NUVOTON TECHNOLOGY CORPORATION. ("NUVOTON") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */




#ifndef _OSUtils_h_
#define _OSUtils_h_

#ifdef __cplusplus
extern "C" {
#endif

    
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                FUNCTION DECLERATION                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

int             PcMemMap(unsigned long *AddressP, unsigned int Size);
unsigned int    PcMaxMapSize(void);
void            PcMemUnmap(unsigned long *AddressP, unsigned int Size);


unsigned char   PcMemReadB (unsigned long Address);
unsigned short  PcMemReadW (unsigned long Address);
unsigned long   PcMemReadD (unsigned long Address);

void            PcMemWriteB(unsigned long Address, unsigned char Data);
void            PcMemWriteW(unsigned long Address, unsigned short Data);
void            PcMemWriteD(unsigned long Address, unsigned long Data);

int             PcMemWrite (unsigned long Address,
                             unsigned long NumOfBytes,
                             int UnitSize,
                             const void* Data);

unsigned char   PcIoReadB  (unsigned long Address);
unsigned short  PcIoReadW  (unsigned long Address);
unsigned long   PcIoReadD  (unsigned long Address);

int             PcIoWriteB (unsigned long Address, unsigned char Data);
int             PcIoWriteW (unsigned long Address, unsigned short Data);
int             PcIoWriteD (unsigned long Address, unsigned long Data);

int             PcIoWrite (unsigned long Address,
                            unsigned long NumOfBytes,
                            int UnitSize,
                            const void* Data);



#ifdef __cplusplus
}                                                      /* closing brace for extern "C" */
#endif

#endif                                                 /* _OSUtils_h_                  */

