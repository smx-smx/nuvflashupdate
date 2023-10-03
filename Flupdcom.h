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



#ifndef FLASH_UPDATE_COMMON_HEADER
#define FLASH_UPDATE_COMMON_HEADER


#define COMMON_RECEIVE_FLASH_CONFIG_FROM_USER
#define COMMON_RECEIVE_FLASH_BIN_IMG_FROM_USER
#define COMMON_SHOW_README_WHEN_NO_PARAMETERS

#define COMMON_WIN_DEFAULT_EXIT_TYPE_BOOT_BLOCK
#define COMMON_DOS_DEFAULT_EXIT_TYPE_BOOT_BLOCK
#define COMMON_CMD_FILE

#define COMMON_FLAG_NOREBOOT
#define COMMON_FLAG_NOPAUSE
#define COMMON_FLAG_CONFIG
#define COMMON_FLAG_V
#define COMMON_FLAG_UP
#define COMMON_FLAG_LPC
#define COMMON_FLAG_C
#define COMMON_FLAG_X
#define COMMON_FLAG_BBL
#define COMMON_FLAG_COMPARE
#define COMMON_FLAG_DUMP
#define COMMON_FLAG_SHF                 /* Assume The System is With Shared Flash */
#define COMMON_FLAG_NSHF                /* Assume The System Is Not With Shared Flash*/
#define COMMON_FLAG_NOCOMP              /* No Comparison Before Download */
#define COMMON_FLAG_NOVERIFY            /* No Verify   */
#define COMMON_FLAG_NT                  /* No Timeout */
#define COMMON_FLAG_DISLUSB
#define COMMON_FLAG_DISSMI
#define COMMON_FLAG_CBA
#define COMMON_FLAG_NOVR
#define COMMON_FLAG_VRD
#define COMMON_FLAG_RFP
#define COMMON_FLAG_OFS
#define COMMON_FLAG_OCV
#define COMMON_FLAG_PFU
#define COMMON_FLAG_GENBSF
#define COMMON_FLAG_RUNBSF
#define COMMON_FLAG_VM
#define COMMON_FLAG_SILENT
#define COMMON_FLAG_BEC

/*-----------------------------------------------------------------------------------------------*/
/* The location of the application appendixes                                                    */
/*-----------------------------------------------------------------------------------------------*/

typedef enum
{

   AEPL_WIN32APP,
   AEPL_IO_MEM_DRIVER_32_BIT,   
   AEPL_IO_MEM_DRIVER_DLL,
   AEPL_IO_MEM_DRIVER_64_BIT,
   AEPL_README,
   AEPL_MESSAGES,
   AEPL_FLASH_CONFIGURATION,
   AEPL_COMPANY_LOGO,
   
} APP_EXE_APPENDIX_LOCATION_TYPE;


/*-----------------------------------------------------------------------------------------------*/
/* In the Windows application, after SFX finished, the driver and it's dll are removed from      */
/* the executable. There for, the Windows application need a different location values           */
/*-----------------------------------------------------------------------------------------------*/

#define GET_APP_EXE_APPENDIX_LOCATION(X)    ((X) - 4)


#endif


