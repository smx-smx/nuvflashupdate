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



/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                          INCLUDES                                             */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#include "App.h"
#include "ErrorCodes.h"

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                  LOCAL VARIABLES                                              */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

AppMessage AppMessages[APP_MAX_RESULT+1] = {

    /* Application Errors */
 { APP_RESULT_OK,                           "APP_RESULT_OK", ""},
 { APP_RESULT_OK_SHOW_STATUS,               "APP_RESULT_OK_SHOW_STATUS", ""}, 
 { APP_RESULT_USER_ABORTED,                 "APP_RESULT_USER_ABORTED", "The flash update process was not completed."},

 { APP_ERR_FLASH_BIN_NOT_SPECIFIED,         "APP_ERR_FLASH_BIN_NOT_SPECIFIED", "Please run the application again and specify a binary image file."},
 { APP_ERR_EMPTY_FLASH_BIN_FILE,            "APP_ERR_EMPTY_FLASH_BIN_FILE", "The given binary image file is empty."},
 { APP_ERR_SMALL_FLASH_CONFIG_FILE,         "APP_ERR_SMALL_FLASH_CONFIG_FILE", "The flash configuration file should be at least 30 bytes."},
 { APP_ERR_CREATING_DEFAULT_CFG_FILE,       "APP_ERR_CREATING_DEFAULT_CFG_FILE", "Failed to create the default flash configuration file. Aborting."},
 { APP_ERR_NO_FLASH_CONFIG_FILE,            "APP_ERR_NO_FLASH_CONFIG_FILE", "Unable to find flash configuration file."}, 
 { APP_ERR_FAILED_COPY_DRIVER_TO_SYSTEM,    "APP_ERR_FAILED_COPY_DRIVER_TO_SYSTEM", "Failed to copy IO/Memory driver to the system directory."}, 
 { APP_ERR_UNRECOGNIZED_PARAMETER,          "APP_ERR_UNRECOGNIZED_PARAMETER", "Unknown parameter %s."},
 { APP_ERR_ILLEGAL_EXIT_TYPE,               "APP_ERR_ILLEGAL_EXIT_TYPE", "Illegal exit type %s."},
 { APP_ERR_CAN_NOT_READ_EXIT_TYPE,          "APP_ERR_CAN_NOT_READ_EXIT_TYPE", "Failed to read the exit type."},
 { APP_ERR_FAILED_TO_OPEN_DRIVER,           "APP_ERR_FAILED_TO_OPEN_DRIVER", "Failed to open the IO/Memory driver."},
 { APP_ERR_FAILED_TO_LOAD_COMPANY_LOGO,     "APP_ERR_FAILED_TO_LOAD_COMPANY_LOGO", ""},
 { APP_ERR_FAILED_TO_LOAD_OEM_LOGO,         "APP_ERR_FAILED_TO_LOAD_OEM_LOGO", ""},
 { APP_ERR_LOGO_NOT_LOADED,                 "APP_ERR_LOGO_NOT_LOADED", "Logo type %d has not been loaded."},
 { APP_ERR_FILE_NOT_EXIST,                  "APP_ERR_FILE_NOT_EXIST", "The file %s does not exist."},
 { APP_ERR_DUMP_FILE_NAME_NOT_SPECIFIED,    "APP_ERR_DUMP_FILE_NAME_NOT_SPECIFIED", "%s must be followed by a file name."},
 { APP_ERR_FLASH_CONFIG_FILE_NOT_SPECIFIED, "APP_ERR_FLASH_CONFIG_FILE_NOT_SPECIFIED", "%s must be followed by a file name."},
 { APP_ERR_BASE_ADDRESS_NOT_SPECIFIED,      "APP_ERR_BASE_ADDRESS_NOT_SPECIFIED", "%s must be followed by an address."},
 { APP_ERR_ILLEGAL_BASE_ADDRESS,            "APP_ERR_ILLEGAL_BASE_ADDRESS", "%s is an illegal base address"},
 { APP_ERR_FAILED_TO_CREATE_FILE,           "APP_ERR_FAILED_TO_CREATE_FILE", ""},
 { APP_ERR_ILLEGAL_FLAG_MIXING,             "APP_ERR_ILLEGAL_FLAG_MIXING", "Mixing between the flags %s is forbidden."}, 
 { APP_ERR_DIFFERENT_BIOS_MODEL,            "APP_ERR_DIFFERENT_BIOS_MODEL", "A different BIOS model is installed on your computer."},
 { APP_ERR_MISSING_BIN_FILE_BIOS_MODEL,     "APP_ERR_MISSING_BIN_FILE_BIOS_MODEL", "Unable to determine the binary file BIOS model."},
 { APP_ERR_MISSING_FLASH_BIOS_MODEL,        "APP_ERR_MISSING_FLASH_BIOS_MODEL", "Unable to determine the BIOS model."},
 { APP_ERR_MISSING_FLASH_BIOS_VERSION,      "APP_ERR_MISSING_FLASH_BIOS_VERSION", "Unable to determine the BIOS version."},
 { APP_ERR_MISSING_BIN_FILE_BIOS_VERSION,   "APP_ERR_MISSING_BIN_FILE_BIOS_VERSION", "Unable to determine the binary file BIOS version."},
 { APP_ERR_MISSING_OFFSET_VALUE,            "APP_ERR_MISSING_OFFSET_VALUE", "Offset address was not specified."},
 { APP_ERR_ILLEGAL_OFFSET_VALUE,            "APP_ERR_ILLEGAL_OFFSET_VALUE", "%s is an illegal offset address."}, 
 { APP_ERR_TSF_NOT_SPECIFIED,               "APP_ERR_TSF_NOT_SPECIFIED", "Text Script File (TSF) was not specified."}, 
 { APP_ERR_BSF_NOT_SPECIFIED,               "APP_ERR_BSF_NOT_SPECIFIED", "Binary Script File (BSF) was not specified."}, 
 { APP_ERR_INVALID_VIDEO_MODE,              "APP_ERR_INVALID_VIDEO_MODE", "Invalid Video Mode value"},
 { APP_ERR_VIDEO_MODE_NOT_SPECIFIED,        "APP_ERR_VIDEO_MODE_NOT_SPECIFIED", "Video Mode flag is missing a value parameter"},
 { APP_ERR_HELP_FILE_NOT_SPECIFIED,         "APP_ERR_HELP_FILE_NOT_SPECIFIED", "%s must be followed by a help file."},
 { APP_ERR_MSG_FILE_NOT_SPECIFIED,          "APP_ERR_MSG_FILE_NOT_SPECIFIED", "%s must be followed by a custom messages file."},
 { APP_ERR_EXEC_FILE_NOT_SPECIFIED,         "APP_ERR_EXEC_FILE_NOT_SPECIFIED", "%s must be followed by a file name."},
 { APP_ERR_HELP_FILE_MISSING,               "APP_ERR_HELP_FILE_MISSING", "Help file %s is missing."},
 { APP_ERR_MSG_FILE_MISSING,                "APP_ERR_MSG_FILE_MISSING", "Messages file %s is missing."},
 { APP_ERR_ILLEGAL_TARGET_EXE_NAME,         "APP_ERR_ILLEGAL_TARGET_EXE_NAME", "Error: Target executable name can't be %s\n"},
 { APP_ERR_TARGET_DRIVE_IS_FULL,            " APP_ERR_TARGET_DRIVE_IS_FULL", "%Target drive doesn't have sufficient free space."}, 
 { APP_ERR_TOO_MANY_PARAMS,                 "APP_ERR_TOO_MANY_PARAMS", "%s cannot be used with -coh, -cmsg or -o."}, 
 { APP_ERR_READING_FROM_FILE,               "APP_ERR_READING_FROM_FILE", "Couldn't read file %s."}, 
 { APP_ERR_OPENING_FILE,                    "APP_ERR_OPENING_FILE", "Couldn't open file %s"}, 
 { APP_ERR_COMPRESSING_FILE,                "APP_ERR_COMPRESSING_FILE", "Couldn't compress file"}, 
 { APP_ERR_MULTIPLE_INSTANCES,              "APP_ERR_MULTIPLE_INSTANCES", "Only a single instance of Flash Update can run at a time."},
    
 { APP_ERR_UNEXPECTED_ERROR,                "APP_ERR_UNEXPECTED_ERROR", "Failed to complete the requested operation."},

 
 /* Application Text */
 
 { APPLICATION_NAME,                        "APPLICATION_NAME", "Nuvoton Flash Update Tool"},
 { MAIN_WINDOW_HEADLINE,                    "MAIN_WINDOW_HEADLINE", "Nuvoton Flash Update Tool - Version "},
 { UPDATE_READY_TO_BEGIN,                   "UPDATE_READY_TO_BEGIN", ""},
 { START_UPDATE_PROCESS_YES_NO,             "START_UPDATE_PROCESS_YES_NO", "Do you want to start the download process?"},
 { START_COMPARISON_PROCESS_YES_NO,         "START_COMPARISON_PROCESS_YES_NO", "Do you want to start the comparison process?"},
 { LOADING_CONFIG_AND_BINARY,               "LOADING_CONFIG_AND_BINARY", "Please wait a few seconds while loading the configuration and binary image files."},
 { STATUS_STARTING_BIOS_UPDATE,             "STATUS_STARTING_BIOS_UPDATE", "Downloading the new image..."},
 { FLASH_UPDATED_SUCCESSFULLY,              "FLASH_UPDATED_SUCCESSFULLY", "The system flash was updated successfully."},
 { PRESS_ANY_KEY_TO_CONTINUE,               "PRESS_ANY_KEY_TO_CONTINUE", "Press any key to continue"},
 { UPDATE_OPERATION_CANCELED,               "UPDATE_OPERATION_CANCELED", "Operation cancelled"},
 { FLASH_UPDATE_NOT_COMPLETED,              "FLASH_UPDATE_NOT_COMPLETED", "The flash update process was not completed."},
 { FU_CORE_ERROR_OCCURRED,                  "FU_CORE_ERROR_OCCURRED", "Failed to complete the requested operation."},
 { WELCOME_TO_THE_APPLICATION,              "WELCOME_TO_THE_APPLICATION", "Welcome to Flash Update Application"},
 { DOWNLOADING_TO_FLASH,                    "DOWNLOADING_TO_FLASH", "Warning: Do not restart or shutdown the computer while downloading to flash."},
 { FLASH_CONFIGURATION_FILE_IS_MISSING,     "FLASH_CONFIGURATION_FILE_IS_MISSING", "Unable to find flash configuration file. Do you want the application to generate the default configuration file?"},
 { STATUS_LOADING_BIOS_IMAGE,               "STATUS_LOADING_BIOS_IMAGE", "Loading Flash image..."},
 { FLASH_UPDATED_SUCCESSFULLY_RESTART,      "FLASH_UPDATED_SUCCESSFULLY_RESTART", "The system flash was updated successfully. Please restart the computer."},
 { FLASH_UPDATED_SUCCESSFULLY_RESTART_WIN,  "FLASH_UPDATED_SUCCESSFULLY_RESTART_WIN", "The system flash was updated successfully.\nDo you want to reboot?"},
 { COMPARING_BIN_FILE_WITH_FLASH,           "COMPARING_BIN_FILE_WITH_FLASH", "Comparing the file %s to the flash data. Please wait a few moments"},
 { FLASH_COMPARE_RESULT,                    "FLASH_COMPARE_RESULT", ""},
 { FLASH_AND_BIN_FILE_ARE_THE_SAME,         "FLASH_AND_BIN_FILE_ARE_THE_SAME", "The file %s matches the flash data."},
 { FLASH_AND_BIN_FILE_ARE_DIFFERENT,        "FLASH_AND_BIN_FILE_ARE_DIFFERENT", "The file %s does not match the flash data."},
 { TARGET_DUMP_FILE_EXIST,                  "TARGET_DUMP_FILE_EXIST", "The target dump file %s already exists. Overwrite it?"},
 { DUMPING_FLASH_TO_FILE,                   "DUMPING_FLASH_TO_FILE", "Dumping flash data to %s. Please wait..."},
 { PLEASE_WAIT,                             "PLEASE_WAIT", "Please wait a few seconds while performing calculations..."},
 { FLASH_PROTECTION_CONTINUE_YES_NO,        "FLASH_PROTECTION_CONTINUE_YES_NO", "The flash contains areas with read/write protection, some of the operations will be ignored. Continue anyway?"},
 { FLASH_BIOS_VER_IS_NEWER_CONTINUE_ANYWAY, "FLASH_BIOS_VER_IS_NEWER_CONTINUE_ANYWAY", "Continue with the installation process?"},
 { APP_BSF_GENERATED_SUCCESSFULLY,          "APP_BSF_GENERATED_SUCCESSFULLY", "Binary Script File was generated successfully"},
 { APP_REPACKED_SUCCESSFULLY,               "APP_REPACKED_SUCCESSFULLY", "Flash update was repacked successfully"},
 { APP_ERR_CUSTOM_MSG_SYNTAX_ERROR,         "APP_ERR_CUSTOM_MSG_SYNTAX_ERROR", "Syntax error while parsing the custom messages text file."},
 { APP_ERR_CUSTOM_MSG_TOO_LONG,             "APP_ERR_CUSTOM_MSG_TOO_LONG", "A custom message cannot exceed %d characters."},
 { BINARY_DUMPED_SUCCESSFULLY,              "BINARY_DUMPED_SUCCESSFULLY", "The system flash was dumped successfully."},
 { APP_MAX_RESULT,                          "APP_MAX_RESULT", ""}
    };


/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                   FUNCTION IMPLEMENTATION                                     */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Function: GetAppErrorString                                                                   */
/*                                                                                               */
/* Parameters:   p_txtCode  - The desired string code                                            */
/*                                                                                               */
/* Returns:      The string of the given APP_RESULT                                              */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:                                                                                  */
/*               Return the appropriate text for the given APP_RESULT code                       */
/*-----------------------------------------------------------------------------------------------*/

char* GetAppTextString(unsigned int p_txtCode)
{

    if (p_txtCode<APP_MAX_RESULT)
    {
        return AppMessages[p_txtCode].messageText;
    }
    else
    {
        return AppMessages[convertGeneralErrorToAppResult(p_txtCode)].messageText;
    }

}




/*-----------------------------------------------------------------------------------------------*/
/* Function: GetMsgBoxTitleString                                                                */
/*                                                                                               */
/* Parameters:   p_appResult - APP_RESULT code                                                   */
/*                                                                                               */
/* Returns:      The title of the given APP_RESULT message box                                   */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:                                                                                  */
/*               Return the appropriate message box title for the given APP_RESULT code          */
/*-----------------------------------------------------------------------------------------------*/

char *GetMsgBoxTitleString(int p_appResult)
{

    switch(p_appResult)
    {

    case APP_ERR_FILE_NOT_EXIST:                        return "Error: File Not Found";
    case APP_ERR_EMPTY_FLASH_BIN_FILE:                  return "Error: Empty File";
    case APP_ERR_CAN_NOT_READ_EXIT_TYPE:                return GetMsgBoxTitleString(APP_ERR_UNEXPECTED_ERROR);
    case APP_ERR_UNRECOGNIZED_PARAMETER:                return "Error: Unrecognized parameter";
    case APP_ERR_SMALL_FLASH_CONFIG_FILE:               return "Error: Flash Configuration File is Too Small";    
    case APP_ERR_CREATING_DEFAULT_CFG_FILE:             return "Failed to Create Configuration File";
    case APP_RESULT_USER_ABORTED:                       return "Operation Canceled";
    case APP_ERR_FLASH_BIN_NOT_SPECIFIED:               return "Error: Binary Image File Was Not Specified";        
    case APP_ERR_ILLEGAL_FLAG_MIXING:                   return "Error: Illegal Flag Mix";
    case APP_ERR_UNEXPECTED_ERROR:                      return "Error: Unexpected Error Occurred";
    
    case START_UPDATE_PROCESS_YES_NO:                   return "Flash Update Process is Ready To Begin";    
    case FLASH_CONFIGURATION_FILE_IS_MISSING:           return "Flash Configuration File is Missing";
    case DOWNLOADING_TO_FLASH:                          return "Download in Progress";
    case WELCOME_TO_THE_APPLICATION:                    return "Welcome to Flash Update Application";
    case FU_CORE_ERROR_OCCURRED:                        return GetMsgBoxTitleString(APP_ERR_UNEXPECTED_ERROR);     
    case FLASH_UPDATE_NOT_COMPLETED:                    return "Update Process Was Not Completed";
    case FLASH_UPDATED_SUCCESSFULLY_RESTART:            return "Finished to Update Flash";
    case COMPARING_BIN_FILE_WITH_FLASH:                 return "Comparing Binary File to the Flash Data";
    case FLASH_COMPARE_RESULT:                          return "Comparison Result";
    case TARGET_DUMP_FILE_EXIST:                        return "File Exists";
    case DUMPING_FLASH_TO_FILE:                         return "Dumping to File";
    case FLASH_PROTECTION_CONTINUE_YES_NO:              return "Flash Protection Warning";
    case APP_RESULT_OK_SHOW_STATUS:                     return "Operation status";
    case APP_BSF_GENERATED_SUCCESSFULLY:                return GetMsgBoxTitleString(APP_RESULT_OK_SHOW_STATUS);
    case APPLICATION_NAME:                              return "Nuvoton Flash Update Tool";
   
    default:                                            return GetMsgBoxTitleString(APPLICATION_NAME);
    }

}



/*-----------------------------------------------------------------------------------------------*/
/* Function: StringToTokens                                                                      */
/*                                                                                               */
/* Parameters:   p_str          - String of tokens                                               */
/*               p_numOfTokens  - Will contain the number of tokens in p_tokes                   */
/*               p_tokens       - Will contain a list of tokens                                  */
/*               p_firstToken   - If not NULL, it will be the first token in p_tokens            */
/*               p_maxTokens    - Maximum number of tokens                                       */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Separate p_str into tokens. The delimiter is the space character                */
/*-----------------------------------------------------------------------------------------------*/

void StringToTokens(char *p_str, int *p_numOfTokens, char ***p_tokens, char *p_firstToken, int p_maxTokens)
{
    char l_delims[] = " ";
    char *l_result = NULL; 
    int l_numOfTokens = 0;
    int l_startIndex = 0;
    


    (*p_tokens) = (char**)(malloc(sizeof(char*) * p_maxTokens));

    if (NULL != p_firstToken)
    {
        l_startIndex = 1;
        l_numOfTokens = 1;
        (*p_tokens)[0] = (char*)(malloc(sizeof(char) * (1 + strlen(p_firstToken))));
        strcpy((*p_tokens)[0], p_firstToken);    
    }
        
    l_result = strtok(p_str, l_delims);
    while((l_result != NULL) && (l_numOfTokens != p_maxTokens))
    {
        (*p_tokens)[l_numOfTokens] = (char*)(malloc(sizeof(char) * (1 + strlen(l_result))));
        strcpy((*p_tokens)[l_numOfTokens], l_result); 
        (*p_tokens)[l_numOfTokens][strlen(l_result)] = 0;
        l_numOfTokens++;        
        l_result = strtok( NULL, l_delims);
    } 
 
    *p_numOfTokens= l_numOfTokens;
    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: IsFileExist                                                                         */
/*                                                                                               */
/* Parameters:   p_fileName          - The file name                                             */
/*                                                                                               */
/* Returns:      0 - The file does not exist or can not be opened for reading from sonme reason  */
/*               1 - File exist                                                                  */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Check if the given file exist and can be open for read                          */
/*-----------------------------------------------------------------------------------------------*/

int IsFileExist(char *p_fileName)
{
    int   l_result = 0;
    FILE *l_fileHandle = NULL;



    l_fileHandle = fopen(p_fileName, "rb");
    if (NULL != l_fileHandle)
    {
        l_result = 1;
        fclose(l_fileHandle);           
    }


    return l_result;
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: GetAppCommandLineFromFile                                                           */
/*                                                                                               */
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      The default command line file content (the first kilobyte)                      */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Open the default command line file and return its content                       */
/*-----------------------------------------------------------------------------------------------*/

char *GetAppCommandLineFromFile()
{
    char            *l_commandLine    = NULL;
    FILE            *l_fileHandle     = NULL;
    size_t           l_numOfBytesRead = 0;
    unsigned int     i                = 0;



    l_fileHandle = fopen(DEFAULT_COMMAND_LINE_FILE, "rb");
    if (NULL !=  l_fileHandle)
    {
        l_commandLine = (char *)(malloc(sizeof(char) * 1024));
        l_numOfBytesRead = fread(l_commandLine, 1, 1023, l_fileHandle);
        if (0 == l_numOfBytesRead)
        {
            free(l_commandLine);
        }           
        else
        {
            l_commandLine[l_numOfBytesRead] = 0; /* Adding terminating zero */

            /* New line char is not allowed */
            
            for (i = 0; i < l_numOfBytesRead; i++)
            {
                if (10 == l_commandLine[i] || 13 == l_commandLine[i])
                {
                    l_commandLine[i] = 0;
                    break;
                }
            }
        }
           
        fclose(l_fileHandle);
    }


    return l_commandLine;   
}

#ifndef __LINUX__
/*--------------------------------------------------------------------------------------*/
/* Function: parseCustomizeMessages                                                     */
/*                                                                                      */ 
/* Parameters:   p_msgFilePtr  - Pointer to the custom messages file                    */
/*               p_msgFileSize - The custom messages file size                          */
/*               toUpdate - Indicate if the message table should also be updated        */
/*                                                                                      */
/* Returns:      APP_RESULT                                                             */
/*                                                                                      */ 
/* Side effects: Could update the Messages array                                        */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Analyze the given custom messages file.                                */
/*                returns an error or quit upon a syntax error in file.                 */
/*                If toUpdate==TRUE: Update the Messages array                          */
/*--------------------------------------------------------------------------------------*/

APP_RESULT parseCustomizeMessages(char *p_msgFilePtr, size_t p_msgFileSize, BOOLEAN toUpdate)
{
    int             l_retVal                    = APP_RESULT_OK;    
    char*           l_strLine;
    char            l_strEnum[100];
    char*           l_message;
    size_t          l_delimiterLocation;
    char            l_enumPrefix[3];

    if (p_msgFileSize <= 0)
    {
        return APP_RESULT_OK;
    }
    
            /* Split file into lines */
            for(l_strLine = strtok(p_msgFilePtr,"\n"); l_strLine != NULL ;l_strLine =strtok(NULL,"\n\r")){
                trim(l_strLine);

                if ((strlen(l_strLine) > 0) && (strcmp(l_strLine,"\n") != 0) && (strcmp(l_strLine,"\r") != 0))   /* Skip empty lines */
                {
                    l_message = strchr(l_strLine, ':');                
                    if (NULL == l_message)
                    {
                        if (TRUE == toUpdate)                        /* Error in pasing should not affect end user */
                        {                                            /*  The user will see the default message     */
                            continue;
                        }
                        else
                        {
                            return APP_ERR_CUSTOM_MSG_SYNTAX_ERROR;
                        }
                    }
                    l_message = l_message + 1;
                    l_delimiterLocation = strcspn(l_strLine,":");                

                    strncpy(l_strEnum, l_strLine, l_delimiterLocation);
                    l_strEnum[l_delimiterLocation] = '\0';

                    trim(l_strEnum);
                    trim(l_message);

                    strncpy(l_enumPrefix,l_strEnum,2);
                    l_enumPrefix[2]='\0';

                    if (0 == strcmpi(l_enumPrefix,"FU")){
                        l_retVal = updateFuMessageTable(l_strEnum, l_message, toUpdate);
                        /* Convert FU Results to APP results */
                        switch (l_retVal)
                        {
                            case FU_RESULT_OK:
                               l_retVal = APP_RESULT_OK;
                               break;
                            case FU_ERR_CUSTOM_MSG_SYNTAX_ERROR:
                               l_retVal = APP_ERR_CUSTOM_MSG_SYNTAX_ERROR;
                               break;
                            case FU_ERR_CUSTOM_MSG_TOO_LONG:
                               l_retVal = APP_ERR_CUSTOM_MSG_TOO_LONG;
                               break;
                            default:
                                l_retVal = APP_ERR_UNEXPECTED_ERROR;
                               break;
                        }
                    }
                    else
                    {
                        l_retVal = updateAppMessageTable(l_strEnum, l_message, toUpdate);
                    }
                    if (APP_RESULT_OK != l_retVal)
                    {

                        if (TRUE == toUpdate)                                   /* Error in pasing should not affect end user */
                        {                                                       /*  The user will see the default message     */
                            continue;
                        }
                        else
                        {
                           return l_retVal;
                        }
                    }
                }
          }  
     if (TRUE == toUpdate)                                                      /* Error in pasing should not affect end user */
    {                                                                           /*  The user will see the default message     */
        return APP_RESULT_OK;
    }
     else
    {
        return l_retVal;
    }
            
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        trim                                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_str - The string to trim                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine trims the given string from spaces and tabs                               */
/*---------------------------------------------------------------------------------------------------------*/
void trim(char * p_str)
{
    size_t i=0;
    size_t j;

/*---------------------------------------------------------------------------------------------------------*/
/* Check if string is 0 length or NULL                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
    if ((0 == strlen(p_str)) || (NULL == p_str))
    {
        return;
    }
/*---------------------------------------------------------------------------------------------------------*/
/* Trim spaces and tabs from beginning:                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
    while((p_str[i]==' ')||(p_str[i]=='\t')) {
        i++;
    }
    if(i>0) {
        for(j=0;j<strlen(p_str);j++) {
            p_str[j]=p_str[j+i];
        }
    p_str[j]='\0';
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Trim spaces, tabs and line breaks from end:                                                             */
/*---------------------------------------------------------------------------------------------------------*/
    i=strlen(p_str)-1;
    while((p_str[i]==' ')||(p_str[i]=='\t')||(p_str[i]== '\n')||(p_str[i]== '\r')) {
        i--;
    }
    if(i<(strlen(p_str)-1)) {
        p_str[i+1]='\0';
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        updateAppMessageTable                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_message - The new message text                                                       */
/*                  p_strEnum - The enum of the message as string                                          */
/*                  toUpdate -  TRUE= update the message table.                                            */
/*                              FALSE= Don't update the message table. just validate it.                   */
/*                                                                                                         */
/* Returns:         APP_RESULT_OK if p_strEnum is valid and the message is valid                           */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine searches for the p_strEnum in the AppMessages array.                      */
/*                  If found and if message is valid (Shorter then MAX_MSG_LENGTH)                         */
/*                  The function will return an APP_RESULT_OK and if needed, update the array.             */
/*---------------------------------------------------------------------------------------------------------*/
APP_RESULT updateAppMessageTable(char* p_strEnum,char*  p_message, BOOLEAN toUpdate)
{
    int i;

    if (strlen(p_message) >= MAX_MSG_LENGTH)
    {
        return APP_ERR_CUSTOM_MSG_TOO_LONG;
    }

    for (i = 0; i<APP_MAX_RESULT; ++i)
    {
      if (strcmpi(p_strEnum, AppMessages[i].enumAsStr) == 0)
        {
            if (TRUE == toUpdate)
            {
                strcpy(AppMessages[i].messageText, p_message);
            }

            return APP_RESULT_OK;
        }
    }
    return APP_ERR_CUSTOM_MSG_SYNTAX_ERROR;
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        convertGeneralErrorToAppResult                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_generalError - General error code from ErrorCodes.h                                  */
/*                                                                                                         */
/* Returns:         The converted AAPP_RESULT                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine converts general error codes from ErrorCodes.h to App error codes         */
/*---------------------------------------------------------------------------------------------------------*/
APP_RESULT convertGeneralErrorToAppResult(int p_generalError){
    switch (p_generalError)
    {
        case ERROR_ILLEGAL_TARGET_EXE_NAME:
           return APP_ERR_ILLEGAL_TARGET_EXE_NAME;
           break;
        case ERROR_READING_FROM_FILE:
           return APP_ERR_READING_FROM_FILE;
           break;
        case ERROR_OPENING_FILE:
           return APP_ERR_OPENING_FILE;
           break;
        case ERROR_HELP_FILE_MISSING:
           return APP_ERR_HELP_FILE_MISSING;
           break;
        case ERROR_MSG_FILE_MISSING:
           return APP_ERR_MSG_FILE_MISSING;
           break;           
        case ERROR_FILE_MISSING:
           return APP_ERR_FILE_NOT_EXIST;
           break;
        case ERROR_TARGET_DRIVE_IS_FULL:
           return APP_ERR_TARGET_DRIVE_IS_FULL;
           break;
        case ERROR_COMPRESSING_FILE:
           return APP_ERR_COMPRESSING_FILE;
           break;
           
        default:
            return APP_ERR_UNEXPECTED_ERROR;
           break;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        getAPPEnumAsString                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_enum - The ENUM number to retrieve                                                   */
/*                                                                                                         */
/* Returns:         The ENUM as string. NULL if p_enum is illegal.                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine return the enum given as its string represntaion.                         */
/*---------------------------------------------------------------------------------------------------------*/
const char* getAPPEnumAsString(int p_enum)
{
    if ((p_enum>=0) && (p_enum <= APP_MAX_RESULT))
    {
        return AppMessages[p_enum].enumAsStr;
    }
    return NULL;
}
    

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        verifyAppArrayConsistency                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         -1 if array is valid. The invalid ENUM otherwise.                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine validates that the messages array enums are equal to the enums            */
/*                  listed in App.h                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int verifyAppArrayConsistency()
{
    int i;

    for (i=0; i<APP_MAX_RESULT+1; ++i)
    {
      if (AppMessages[i].enumAsInt != i)
      {
        return i;
      }
    }

    return -1;
}




