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


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "FUresult.h" 

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                  LOCAL VARIABLES                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
FuMessage FuMessages[FU_MAX_RESULT+1] = {

 { FU_RESULT_OK, "FU_RESULT_OK", ""},
 { FU_RESULT_USER_ABORTED, "FU_RESULT_USER_ABORTED", ""},
 { FU_RESULT_NO_BIOS_MODEL, "FU_RESULT_NO_BIOS_MODEL", ""},
 { FU_RESULT_OLDER_BIN_FILE_BIOS, "FU_RESULT_NO_BIOS_MODEL", ""},
 
 { FU_ERR_FLASH_CONFIG_FILE_TOO_SMALL,          "FU_ERR_FLASH_CONFIG_FILE_TOO_SMALL", "Flash configuration file is too small."},
 { FU_ERR_UNEXPECTED_FLASH_ID,                  "FU_ERR_UNEXPECTED_FLASH_ID", "Unexpected flash ID."},
 { FU_ERR_DOWNLOADING_TO_FLASH,                 "FU_ERR_DOWNLOADING_TO_FLASH", "Error downloading to flash."},
 { FU_ERR_MEMORY_MAP_FAILED,                    "FU_ERR_MEMORY_MAP_FAILED", "Memory mapping have been failed."},
 { FU_ERR_FIRMWARE_ERROR,                       "FU_ERR_FIRMWARE_ERROR", "Firmware error."},
 { FU_ERR_SHM_NOT_CONFIG,                       "FU_ERR_SHM_NOT_CONFIG", "Shared memory has not been configured."},
 { FU_ERR_ILLEGAL_FLASH_SIZE,                   "FU_ERR_ILLEGAL_FLASH_SIZE", "Illegal flash size."},
 { FU_ERR_BASE_ADDRESS_NOT_ALIGNED,             "FU_ERR_BASE_ADDRESS_NOT_ALIGNED", "Base address not aligned."},
 { FU_ERR_DEVICE_NOT_FOUND,                     "FU_ERR_DEVICE_NOT_FOUND", "Device not found."},
 { FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES,    "FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES", "Flash Configuration file contains wrong values."},
 { FU_ERR_FAIL_PROGRAMING_ADDRESS,              "FU_ERR_FAIL_PROGRAMING_ADDRESS", "Failed to program address."},
 { FU_ERR_TO_MUCH_BYTES_TO_PROGRAM,             "FU_ERR_TO_MUCH_BYTES_TO_PROGRAM", "To much byte to program."},
 { FU_ERR_CANT_FIND_CORRECT_FLASH_CONFIGURATION,"FU_ERR_CANT_FIND_CORRECT_FLASH_CONFIGURATION", "Unable to find a correct flash configuration."},
 { FU_ERR_GIVEN_NULL_POINTER,                   "FU_ERR_GIVEN_NULL_POINTER", ""},
 { FU_ERR_MISSING_BIN_FILE_BIOS_MODEL,          "FU_ERR_MISSING_BIN_FILE_BIOS_MODEL", ""},
 { FU_ERR_MISSING_FLASH_BIOS_MODEL,             "FU_ERR_MISSING_FLASH_BIOS_MODEL", ""},
 { FU_ERR_DIFFERENT_BIOS_MODEL,                 "FU_ERR_DIFFERENT_BIOS_MODEL", ""},
 { FU_ERR_MISSING_BIN_FILE_BIOS_VERSION,        "FU_ERR_MISSING_BIN_FILE_BIOS_VERSION", ""},
 { FU_ERR_MISSING_FLASH_BIOS_VERSION,           "FU_ERR_MISSING_FLASH_BIOS_VERSION", ""},
 { FU_ERR_TRY_TO_READ_FROM_PROTECTED_AREA,      "FU_ERR_TRY_TO_READ_FROM_PROTECTED_AREA", "Attempted to read from a flash protected area."},
 { FU_ERR_OFFSET_DONT_DESCRIBE_A_SECTOR_START,  "FU_ERR_OFFSET_DONT_DESCRIBE_A_SECTOR_START", ""},
 { FU_ERR_SCR_INVALID_SECTION,                  "FU_ERR_SCR_INVALID_SECTION", "Invalid or missing Script section"},
 { FU_ERR_SCR_INVALID_COMMAND,                  "FU_ERR_SCR_INVALID_COMMAND", "Invalid Script command"}, 
 { FU_ERR_SCR_FILE_OPEN_ERROR,                  "FU_ERR_SCR_FILE_OPEN_ERROR", "Couldn't open Script file"}, 
 { FU_ERR_SCR_INVALID_TSF_NAME_ERROR,           "FU_ERR_SCR_INVALID_TSF_NAME_ERROR", "Invalid Text Script File name"}, 
 { FU_ERR_SCR_SYNTAX_ERROR,                     "FU_ERR_SCR_SYNTAX_ERROR", "Syntax error in Text Script File"},
 { FU_ERR_SCR_COMMAND_NOT_SUPPORTED_IN_x64,     "FU_ERR_SCR_COMMAND_NOT_SUPPORTED_IN_x64", "The command is not supported under x64 systems"},
 { FU_ERR_CUSTOM_MSG_SYNTAX_ERROR,              "FU_ERR_CUSTOM_MSG_SYNTAX_ERROR", "Syntax error while parsing the custom messages text file."},
 { FU_ERR_CUSTOM_MSG_TOO_LONG,                  "FU_ERR_CUSTOM_MSG_TOO_LONG", "A custom message cannot exceed %d."},    

 
 { FU_ERR_UNEXPECTED_ERROR,                     "FU_ERR_UNEXPECTED_ERROR", "Unexpected error. Failed to update flash."},

 { FU_MAX_RESULT,                               "FU_MAX_RESULT", ""},
    };

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             FUNCTION IMPLEMENTATION                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function: GetFUErrorString                                                           */
/*                                                                                      */ 
/* Parameters:   p_fuResult       - The desired FU_RESULT code                          */
/*                                                                                      */
/* Returns:      The string of the given FU_RESULT                                      */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Returning the appropriate text for the given FU_RESULT                 */
/*--------------------------------------------------------------------------------------*/

char *GetFUErrorString(FU_RESULT p_fuResult)
{

    if (p_fuResult<FU_MAX_RESULT)
    {
        return FuMessages[p_fuResult].messageText;
    }
    else
    {
        return NULL;
    }

}
#ifndef __LINUX__
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        updateFuMessageTable                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_message - The new message text                                                       */
/*                  p_strEnum - The enum of the message as string                                          */
/*                  toUpdate -  TRUE= update the message table.                                            */
/*                              FALSE= Don't update the message table. just validate it.                   */
/*                                                                                                         */
/* Returns:         FU_RESULT_OK if p_strEnum is valid and the message is valid                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine searches for the p_strEnum in the FuMessages array.                       */
/*                  If found and if message is valid (Shorter then MAX_MSG_LENGTH)                         */
/*                  The function will return an FU_RESULT_OK and if needed, update the array.              */
/*---------------------------------------------------------------------------------------------------------*/
FU_RESULT updateFuMessageTable(char* p_strEnum,char*  p_message, BOOLEAN toUpdate)
{
    int i;

    if (strlen(p_message) >= MAX_MSG_LENGTH)
    {
        return FU_ERR_CUSTOM_MSG_TOO_LONG;
    }

    for (i = 0; i<FU_MAX_RESULT; ++i)
    {
      if (strcmpi(p_strEnum, FuMessages[i].enumAsStr) == 0)
        {
            if (TRUE == toUpdate)
            {
                strcpy(FuMessages[i].messageText, p_message);

            }

            return FU_RESULT_OK;
        }
    }
    return FU_ERR_CUSTOM_MSG_SYNTAX_ERROR;
}
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        getFUEnumAsString                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_enum - The ENUM number to retrieve                                                   */
/*                                                                                                         */
/* Returns:         The ENUM as string. NULL if p_enum is illegal.                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine return the enum given as its string represntaion.                         */
/*---------------------------------------------------------------------------------------------------------*/
const char* getFUEnumAsString(int p_enum)
{
    if ((p_enum>=0) && (p_enum <= FU_MAX_RESULT))
    {
        return FuMessages[p_enum].enumAsStr;
    }
    return NULL;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        verifyFuArrayConsistency                                                               */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         -1 if array is valid. The invalid ENUM otherwise.                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine validates that the messages array enums are equal to the enums            */
/*                  listed in FUresult.h                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
int verifyFuArrayConsistency()
{
    int i;

    for (i=0; i< FU_MAX_RESULT+1; ++i)
    {
      if (FuMessages[i].enumAsInt != i)
      {
        return i;
      }
    }

    return -1;
}

