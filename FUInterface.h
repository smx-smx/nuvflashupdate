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


#ifndef FUINTERFACE_HEADER 
#define FUINTERFACE_HEADER

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FUCore.h"


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                     DEFINITIONS                                      */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#define MAX_FLASH_CONFIGURATIONS     100       /* Maximum number of flash               */
                                               /* configurations FUcore can work with   */


#define BIOS_MODEL_OFFSET_SIGNATURE             "$AD"
#define BIOS_MODEL_SIGNATURE                    "$ML"
#define BIOS_VERSION_SIGNATURE                  "$VR"


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                FUNCTION DECLERATIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


FU_RESULT GetCorrectFlashConfiguration(FlashUpdateOptions *p_flashUpdateOptions,
                                           FlashConfigurationData **p_flashConfig);


FU_RESULT CompareWithFlashContent(char *p_filePtr, size_t p_fileSize, int *p_sameFlag, long p_comparisonOffset);

FU_RESULT            AnalyzeConfigurationFile(char * p_configFilePtr, size_t p_configFileSize);

FU_RESULT            ConfigureFlashAccess(FlashUpdateOptions *p_flashUpdateOptions);

FlashUpdateOptions*  GetDefaultFlashUpdateOptions();

FU_RESULT AddSegmentAsProtectedSectors(int p_segmentNumber,
                                           int p_segmentSize,
                                           SECTOR_PROTECTION_TYPE p_protectionType,
                                           FlashUpdateOptions *p_flashUpdateOption);


FU_RESULT IsSameBIOSModel(char *p_binaryImageFilePtr, char **p_binModel, char **p_flashModel);
FU_RESULT IsFlashBIOSNewer(char *p_binaryImageFilePtr, char **p_binVer, char **p_flashVer);

                                     

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif

