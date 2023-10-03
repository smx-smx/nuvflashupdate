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
/*                                            INCLUDES                                           */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#ifndef APPDISPLAY_HEADER 
#define APPDISPLAY_HEADER

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifndef __LINUX__
#include "graph.h"
#endif
#include "App.h"
#include "FUresult.h"
#ifndef __LINUX__
#include "Bmp.h"
#include "Exe_archive.h"
#endif

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                 EXTERNAL FUNCTION DECLARATION                                 */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

extern char* itoa();
extern int getch();

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                       TYPE DEFINITIONS                                        */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#define APP_DEFAULT_PRINT                           0
#define APP_VERBOSE_PRINT                           1
#define APP_ERROR_PRINT                             2


#define APP_DEFAULT_LOG_FILENAME                    "update.log"

/*-----------------------------------------------------------------------------------------------*/
/* Indicators whether the screen display is in graphic mode or not                               */
/*-----------------------------------------------------------------------------------------------*/

#define GRAPHIC_MODE_ON             1
#define GRAPHIC_MODE_OFF            0

/*-----------------------------------------------------------------------------------------------*/
/* This type is used in AppExit to determine the type of the error code                          */
/*-----------------------------------------------------------------------------------------------*/

typedef enum
{
    
    FU_RESULT_TYPE,
    APP_RESULT_TYPE

} ERROR_CODE_TYPE;


typedef enum
{
    
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_CENTER
    
} TEXT_ALIGN_TYPE;


typedef enum
{
    
    MSG_BOX_GENERAL,
    MSG_BOX_ERR,
    MSG_BOX_INFO,
    MSG_BOX_YES_NO
    
} MSG_BOX_TYPE;


typedef enum
{

    OEM_LOGO,
    COMPANY_LOGO
    
}LOGO_TYPE;

typedef enum
{
    
    REPORT_ALL_ERRORS = 1,
    REPORT_ONLY_APP_ERRORS

} ERROR_REPORT_LEVEL;




/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                     FUNCTION DECLERATIONS                                     */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

APP_RESULT LoadCompanyLogo(unsigned char *p_bmpPtr, unsigned long p_bmpFileSize);
APP_RESULT LoadOEMLogo(unsigned char *p_bmpPtr, unsigned long p_bmpFileSize);
APP_RESULT LoadLogo(unsigned char *p_bmpPtr, unsigned long p_bmpFileSize, LOGO_TYPE p_logoType);


void SetMainWindowTopTitle(char *p_title);
void SetMainWindowBottomTitle(char *p_title);

void DisplayCenterMsgBox(MSG_BOX_TYPE p_msgBoxType,
                            char *p_title,
                            int p_width,
                            char p_displayPressKeyToContinue);

void PrintCenterMsgBoxText(int p_top,
                           TEXT_ALIGN_TYPE p_align,
                           char *p_textToPrint,
                           ...);

void EnterGraphicMode(short p_videoMode);
void ExitGraphicMode();

void SetErrorReportLevel(ERROR_REPORT_LEVEL p_reportLevel);

void AppExit(int p_errorCode, ERROR_CODE_TYPE p_errorType, char *p_additionalText, ...);
void FillProgressBar(unsigned int p_precent);
void UpdateProgress(int p_precent);

void PrintText(int p_printType, char *p_textToPrint, ...);

void StartingFlashConfiguration();
void SetLogLevel(int p_level);
int  GetGraphicMode();

#endif


