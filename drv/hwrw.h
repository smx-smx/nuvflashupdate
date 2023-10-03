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



typedef struct GeneralParam_T {
   unsigned long p_1;
   unsigned long p_2;
} GeneralParam_T ;


/*
 * Ioctl definitions
 */

/* Use 'h' as magic number */
#define HWRW_IOC_MAGIC  'h'

#define HWRW_IOC_MEM_MAP                    _IO(HWRW_IOC_MAGIC, 0)

#define HWRW_IOC_MEM_READ_32                _IO(HWRW_IOC_MAGIC, 1)
#define HWRW_IOC_MEM_WRITE_32               _IO(HWRW_IOC_MAGIC, 2)
#define HWRW_IOC_MEM_READ_16                _IO(HWRW_IOC_MAGIC, 3)
#define HWRW_IOC_MEM_WRITE_16               _IO(HWRW_IOC_MAGIC, 4)
#define HWRW_IOC_MEM_READ_8                 _IO(HWRW_IOC_MAGIC, 5)
#define HWRW_IOC_MEM_WRITE_8                _IO(HWRW_IOC_MAGIC, 6)

#define HWRW_IOC_IO_READ_8                  _IO(HWRW_IOC_MAGIC, 7)
#define HWRW_IOC_IO_WRITE_8                 _IO(HWRW_IOC_MAGIC, 8)
#define HWRW_IOC_IO_READ_16                 _IO(HWRW_IOC_MAGIC, 9)
#define HWRW_IOC_IO_WRITE_16                _IO(HWRW_IOC_MAGIC, 10)
#define HWRW_IOC_IO_READ_32                 _IO(HWRW_IOC_MAGIC, 11)
#define HWRW_IOC_IO_WRITE_32                _IO(HWRW_IOC_MAGIC, 12)

#define HWRW_IOC_MEM_UNMAP                    _IO(HWRW_IOC_MAGIC, 13)

