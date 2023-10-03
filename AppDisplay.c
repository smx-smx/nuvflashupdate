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
/*                                           INCLUDES                                            */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#include "AppDisplay.h"
#include "FUCore.h"
#include "exe_archive.h"

#ifdef __LINUX__
#include <stdio.h>
#define getch()   getchar()
#endif
/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                          DEFINITIONS                                          */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/


/* Regular Text Colors */

#define T_BLACK                     0x000000L
#define T_BLUE                      0x2a0000L
#define T_GREEN                     0x002a00L
#define T_CYAN                      0x2a2a00L
#define T_RED                       0x00002aL
#define T_MAGENTA                   0x2a002aL
#define T_BROWN                     0x00152aL
#define T_WHITE                     0x2a2a2aL
#define T_GRAY                      0x151515L
#define T_LIGHTBLUE                 0x3F1515L
#define T_LIGHTGREEN                0x153f15L
#define T_LIGHTCYAN                 0x3f3f15L
#define T_LIGHTRED                  0x15153fL
#define T_LIGHTMAGENTA              0x3f153fL
#define T_YELLOW                    0x153f3fL
#define T_BRIGHTWHITE               0x3f3f3fL


/* Graphic Colors */

#define G_BLACK                     0
#define G_BLUE                      1
#define G_GREEN                     2
#define G_CYAN                      3
#define G_RED                       4
#define G_MAGENTA                   5
#define G_YELLOW                    6
#define G_WHITE                     7
#define G_GRAY                      8
#define G_LIGHT_BLUE                9
#define G_LIGHT_GREEN               10
#define G_LIGHT_CYAN                11
#define G_LIGHT_RED                 12
#define G_LIGHT_MAGENTA             13
#define G_LIGHT_YELLOW              14
#define G_BRIGHT_WHITE              15

#define G_WINDOW_WIDTH              640
#define G_WINDOW_HEIGHT             480
#define G_WINDOW_FRAME_SPACE        3
#define G_WINDOW_FRAME_OFFSET       2

#define T_WINDOW_WIDTH              80
#define T_WINDOW_HEIGHT             30

#define TEXT_BACK_COLOR             T_BLUE
#define TEXT_FONT_COLOR             G_WHITE

#define PROGRESS_BAR_BACK_COLOR     G_WHITE
#define PROGRESS_BAR_FILL_COLOR     G_LIGHT_YELLOW
#define PROGRESS_BAR_BORDER_COLOR   G_GRAY


/*-----------------------------------------------------------------------------------------------*/
/* Determines the maximum number of characters printed per line in printTextMode()               */
/*-----------------------------------------------------------------------------------------------*/
#define MAX_CHARS_PER_LINE          60

#define SIZE_OF_PROGRESS_BAR 13

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                  LOCAL VARIABLE DECLARATION                                   */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

static int  m_logLevel       = 0;                     /* 1: Log only verbose prints              */
                                                       /* 2: Log all prints                      */
                                                       /* else log disabled                      */
                                               
 
static int  m_logCounter     = 0;                    /* Hold the number of times we added        */
                                                       /* text to the application log file       */

/*-----------------------------------------------------------------------------------------------*/
/* m_isScreenInGraphicMode                                                                       */
/*                                                                                               */
/* GRAPHIC_MODE_ON:  Screen is not in graphic mode                                               */
/*                   Don't use any of the graphic functions                                      */
/*                                                                                               */
/* GRAPHIC_MODE_OFF: The screen is in graphic mode                                               */
/*                   Don't use printf and etc                                                    */
/*-----------------------------------------------------------------------------------------------*/

static int m_isScreenInGraphicMode = GRAPHIC_MODE_OFF;                                              


/* These two strings will hold the Main window titles */

static char         m_mainWindowTopTitle[100];
static char         m_mainWindowBottomTitle[100];

#ifndef __LINUX__
static MSG_BOX_TYPE m_currentCenterMsgBoxType  = MSG_BOX_GENERAL;
static int          m_currentCenterMsgBoxWidth = 0;
static BmpFile    *m_companyLogoBmpFile = NULL;
static BmpFile    *m_OEMLogoBmpFile     = NULL;
#endif
static char         m_errorReportLevel   = REPORT_ONLY_APP_ERRORS;

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                  LOCAL FUNCTION DECLARATION                                   */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/
#ifndef __LINUX__
static void drawWindow(int p_left,
                       int p_top,
                       int p_width,
                       int p_height,
                       short p_backColor,
                       short p_textColor,
                       long p_textBackColor,
                       char *p_title,
                       char *p_bottomTitle,
                       short p_titleBackColor);


static APP_RESULT drawLogo(unsigned char p_transparentColor,
                            char p_transparent,
                            unsigned char p_pixelColor,
                            LOGO_TYPE p_logoType);
#endif


static void mergeText(char *p_text,...);

static BOOLEAN printTextMode(char *p_str);
static void SetProgressBar(unsigned int p_percent, char* p_progBar);

static void addToLogFile(char *p_logText);
/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                    FUNCTION IMPLEMENTATION                                    */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Function: SetErrorReportLevel                                                                 */
/*                                                                                               */
/* Parameters:   p_reportLevel   -  ERROR_REPORT_LEVEL                                           */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: set m_errorReportLevel to be the given error report level                       */
/*                                                                                               */
/* Description:                                                                                  */
/*               This function will set the user error report level                              */
/*-----------------------------------------------------------------------------------------------*/

void SetErrorReportLevel(ERROR_REPORT_LEVEL p_reportLevel)
{
    m_errorReportLevel = p_reportLevel;
}

#ifdef __LINUX__
APP_RESULT LoadLogo(unsigned char *p_bmpPtr, unsigned long p_bmpFileSize, LOGO_TYPE p_logoType)
{
    return APP_RESULT_OK;
}
#else
/*-----------------------------------------------------------------------------------------------*/
/* Function: LoadLogo                                                                            */
/*                                                                                               */
/* Parameters:   p_bmpPtr      - A pointer to the mapped bitmap file                             */
/*               p_bmpFileSize - The bitmap file size                                            */
/*               p_logoType    - The type of logo we want to load                                */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: set the appropriate bmpFile static variable that corresponded to the given      */
/*               logo type                                                                       */
/*                                                                                               */
/* Description:                                                                                  */
/*               This function will load the given bitmap file into the appropriate BmpFile      */
/*               structure corresponded to the given logo type                                   */
/*-----------------------------------------------------------------------------------------------*/

APP_RESULT LoadLogo(unsigned char *p_bmpPtr, unsigned long p_bmpFileSize, LOGO_TYPE p_logoType)
{
    APP_RESULT l_retVal = APP_RESULT_OK;
    int l_errorType = 0;
    BmpFile **l_logoFilePtr = NULL;
    

    
    switch(p_logoType)
    {
        
    case OEM_LOGO:
        l_logoFilePtr = &m_OEMLogoBmpFile;
        l_errorType = APP_ERR_FAILED_TO_LOAD_OEM_LOGO;
        break;
        
    case COMPANY_LOGO:
        l_logoFilePtr = &m_companyLogoBmpFile;
        l_errorType = APP_ERR_FAILED_TO_LOAD_COMPANY_LOGO;
        break;
        
    default:
        return l_retVal;
    }
    
    if ((0 == p_bmpFileSize) || (NULL == p_bmpPtr))
    {
        addToLogFile( "NO BMP\n");
        return l_errorType;
    }
    
    l_retVal = LoadBMPFile(NULL, p_bmpPtr, p_bmpFileSize, l_logoFilePtr);
    if (APP_RESULT_OK != l_retVal)
    {      
        return l_errorType;
    }    
   
    return l_retVal;
}
#endif


#ifndef __LINUX__
/*-----------------------------------------------------------------------------------------------*/
/* Function: drawLogo                                                                            */
/*                                                                                               */
/* Parameters:   p_transparentColor - This color will be referred as transparent color           */
/*               p_transparent      - If not 0 do not show transparent color                     */
/*               p_pixelColor       - In case of 1-bit bitmaps. This color will represent        */
/*                                    painted pixel                                              */
/*               p_logoType         - The type of the logo (Company, OEM)                        */
/*                                                                                               */
/* Returns:      APP_RESULT                                                                      */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:  Draw the selected logo at a fixed place on the graphic screen                   */
/*-----------------------------------------------------------------------------------------------*/

static APP_RESULT drawLogo(unsigned char p_transparentColor,
                            char p_transparent,
                            unsigned char p_pixelColor,
                            LOGO_TYPE p_logoType)
                                    
{
    APP_RESULT    l_retVal       = APP_RESULT_OK;
    unsigned long x              = 0;
    unsigned long y              = 0;
    unsigned char *l_bmpRawData  = NULL;
    unsigned int  l_currentBit   = 1;
    int           l_left         = 0;
    int           l_top          = 0;
    BmpFile      *l_logoBmpFile = NULL;
    
    
   

    switch(p_logoType)
    {
    case COMPANY_LOGO: 
        l_left = G_WINDOW_WIDTH - m_companyLogoBmpFile->Header->Width - 40;
        l_top = G_WINDOW_HEIGHT - m_companyLogoBmpFile->Header->Height - 40;
        l_logoBmpFile = m_companyLogoBmpFile;
        break;
        
    case OEM_LOGO:
        l_left = 40;
        l_top = G_WINDOW_HEIGHT - m_OEMLogoBmpFile->Header->Height - 40;
        l_logoBmpFile = m_OEMLogoBmpFile;
        break;            
    }
    
    if (NULL == l_logoBmpFile)
    {
        return APP_ERR_LOGO_NOT_LOADED;
    }    

    for (y = 0; y < l_logoBmpFile->Header->Height; y++)
    {
        for (x = 0; x < l_logoBmpFile->Header->Width; x++)        
        {

            /*-----------------------------------------------------------------------------------*/
            /* Drawing the pixels                                                                */
            /* In case of transparent, if the color of the pixel is like the given transparent   */
            /* color, there is no need to draw the pixel                                         */
            /*-----------------------------------------------------------------------------------*/

            if (1 == l_logoBmpFile->Header->NumOfBitsPerPixel)
            {
                _setcolor(p_pixelColor);
                if (0 == (*(l_logoBmpFile->PixelData + x + (l_logoBmpFile->Header->Width * y))))
                {
                    _setpixel(l_left + x, l_top + y);
                }
            }
            else if ((!p_transparent) || 
                    (*(l_logoBmpFile->PixelData + x + (l_logoBmpFile->Header->Width * y))) != 
                     p_transparentColor)
            {
                _setcolor((*(l_logoBmpFile->PixelData + x + (l_logoBmpFile->Header->Width * y))));
                _setpixel(l_left + x, l_top + y);           
            }              
        }
    }

 
    return l_retVal;    
}
#endif

#ifdef __LINUX__
void DisplayCenterMsgBox(MSG_BOX_TYPE p_msgBoxType,
                            char *p_title,
                            int p_width,
                            char p_displayPressKeyToContinue)                            
{
    printTextMode(p_title);
    return;
}

#else
/*-----------------------------------------------------------------------------------------------*/
/* Function: DisplayCenterMsgBox                                                                 */
/*                                                                                               */
/* Parameters:   p_msgBoxType                - The message box type                              */
/*               p_title                     - Message box title                                 */
/*               p_width                     - Message box width                                 */
/*               p_displayPressKeyToContinue - If not 0 display "Press any key to continue"      */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: set m_currentCenter static variables                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               This function will display a message box in the center of the screen            */
/*-----------------------------------------------------------------------------------------------*/

void DisplayCenterMsgBox(MSG_BOX_TYPE p_msgBoxType,
                            char *p_title,
                            int p_width,
                            char p_displayPressKeyToContinue)                            
{
    int   l_boxWidth        = 0;
    int   l_boxHeight       = 113;
    int   l_boxLeft         = 0;
    int   l_boxTop          = 0;
    int   l_titleLeft       = 0;
    short l_msgBoxBackColor = 0;    
    long  l_txtBackColor    = 0;


    if (GRAPHIC_MODE_OFF == m_isScreenInGraphicMode)
    {
        printTextMode(p_title);
        return;
    }

    if (0 == p_width)
    {
        p_width = 100 + strlen(p_title) * (G_WINDOW_WIDTH/ T_WINDOW_WIDTH); 
        p_width = (p_width < 250) ? 250 : p_width;
    }

    l_boxWidth  = p_width;
    l_boxLeft   = G_WINDOW_WIDTH / 2 - l_boxWidth / 2;
    l_boxTop    = G_WINDOW_HEIGHT / 2 - l_boxHeight / 2;
    l_titleLeft = T_WINDOW_WIDTH / 2 - (strlen(p_title) + 2)/ 2;
    
    m_currentCenterMsgBoxType  = p_msgBoxType;
    m_currentCenterMsgBoxWidth = p_width;

    switch (p_msgBoxType)
    {
    case MSG_BOX_ERR:
        l_msgBoxBackColor = G_RED;
        l_txtBackColor    = T_RED;
        break;
        
    default:
        l_msgBoxBackColor = G_BLUE;
        l_txtBackColor    = T_BLUE;
        break;
    }

    
    drawWindow(0,
               0,
               G_WINDOW_WIDTH,
               G_WINDOW_HEIGHT,
               G_WHITE, G_WHITE,
               0,
               m_mainWindowTopTitle,
               NULL,
               l_msgBoxBackColor);

       
    _setbkcolor(l_txtBackColor);
    
    /* Draw the center window shadow */

    _setcolor(G_GRAY);        
    _rectangle(_GFILLINTERIOR,
               l_boxLeft + 6,
               l_boxTop + 6,
               l_boxLeft + l_boxWidth + 14,
               l_boxTop + l_boxHeight + 14);

    /* Draw the center window */
        
    _setcolor(l_msgBoxBackColor);
    _rectangle(_GFILLINTERIOR,
               l_boxLeft - 8,
               l_boxTop - 8,
               l_boxLeft + l_boxWidth + 8,
               l_boxTop + l_boxHeight + 8);
    
    /* Create the message box frame */
    
    _setcolor(G_BRIGHT_WHITE);
    _rectangle(_GBORDER,
               l_boxLeft,
               l_boxTop,
               l_boxLeft + l_boxWidth,
               l_boxTop + l_boxHeight);               
               
    _rectangle(_GBORDER,
               l_boxLeft - 3,
               l_boxTop - 3,
               l_boxLeft + l_boxWidth + 3,
               l_boxTop + l_boxHeight + 3);

    
    /* Print the message box title */
    
    _settextposition(12 , T_WINDOW_WIDTH / 2 - (strlen(p_title) + 2) / 2 + 1);
    _settextcolor(G_BRIGHT_WHITE);
    _outtext(" ");    
    _outtext(p_title);    
    _outtext(" ");

    if (MSG_BOX_YES_NO == p_msgBoxType)
    {
        PrintCenterMsgBoxText(4, TEXT_ALIGN_CENTER, "(Y = Yes   N = No)");
    }
    else if (p_displayPressKeyToContinue)
    {
        _settextposition(18 , T_WINDOW_WIDTH / 2 - (strlen(GetAppTextString(PRESS_ANY_KEY_TO_CONTINUE)) + 2) / 2 + 1);
        _outtext(GetAppTextString(PRESS_ANY_KEY_TO_CONTINUE));
    }

    
    drawLogo(G_BRIGHT_WHITE, 1, G_RED, COMPANY_LOGO);
    drawLogo(G_BRIGHT_WHITE, 1, G_RED, OEM_LOGO);
}
#endif

#ifdef __LINUX__
void PrintCenterMsgBoxText(int p_top,
                           TEXT_ALIGN_TYPE p_align,
                           char *p_textToPrint,
                           ...)
{
    char l_txtBuffer[256];    
    va_list l_argptr;

    /*--------------------------------------------------------------------------------------*/
    /*      Copy the arguments into the buffer for future printing                          */
    /*--------------------------------------------------------------------------------------*/
    va_start(l_argptr, p_textToPrint);
    vsprintf(l_txtBuffer, p_textToPrint, l_argptr);
    va_end(l_argptr);

    printTextMode(l_txtBuffer);
    return;
}
#else
/*-----------------------------------------------------------------------------------------------*/
/* Function: PrintCenterMsgBoxText                                                               */
/*                                                                                               */ 
/* Parameters:   p_txt  - Text to print                                                          */
/*               p_top  - A value between 0 and 4. The text top inside the message box           */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Print the given text inside the message box. A message box                      */
/*               need to be created before calling this function.                                */ 
/*-----------------------------------------------------------------------------------------------*/

void PrintCenterMsgBoxText(int p_top,
                           TEXT_ALIGN_TYPE p_align,
                           char *p_textToPrint,
                           ...)
{
    int i = 0;
    int l_numOfCharsInLine = (m_currentCenterMsgBoxWidth - 30) / 
                             ((int)G_WINDOW_WIDTH / (int)T_WINDOW_WIDTH);
    char l_currentLine[100];
    char l_txtBuffer[256];    
    va_list l_argptr;
    int l_lineEnd = -1;
    int l_lineStart = 0;
    int l_currentTop = 14 + p_top;
    int l_txtLen = 0;

    /*--------------------------------------------------------------------------------------*/
    /*      Copy the arguments into the buffer for future printing                          */
    /*--------------------------------------------------------------------------------------*/
    va_start(l_argptr, p_textToPrint);
    vsprintf(l_txtBuffer, p_textToPrint, l_argptr);
    va_end(l_argptr);

    if (GRAPHIC_MODE_OFF == m_isScreenInGraphicMode)
    {
        printTextMode(l_txtBuffer);
        return;
    }

    
    l_txtLen = strlen(l_txtBuffer);
    
    if (NULL == p_textToPrint)
    {
        _settextposition(l_currentTop , T_WINDOW_WIDTH / 2 - l_numOfCharsInLine / 2) ;          
        for (i = 0; i < l_numOfCharsInLine; i++)
        {
            _outtext(" ");
        }
        _outtext("\n");
    }
    else
    {
        _settextcolor(G_BRIGHT_WHITE);
        while ((l_txtLen - l_lineEnd > 0))
        {            
            l_lineStart = l_lineEnd + 1;            
            if (l_lineStart + l_numOfCharsInLine >= l_txtLen)
            {
                l_lineEnd = l_txtLen;
            }
            else
            {
                for (i = l_lineStart + l_numOfCharsInLine; i > l_lineStart; i--)
                {
                    if (' ' == l_txtBuffer[i])
                    {
                        l_lineEnd = i;
                        break;
                    }
                }
            }

            for (i = 0; i < l_lineEnd - l_lineStart; i++)
            {
                l_currentLine[i] = l_txtBuffer[l_lineStart + i];
            }
            l_currentLine[i] = 0;                                   /* Add teminating zero */
            
            
            switch (p_align)
            {
            case TEXT_ALIGN_LEFT:
                _settextposition(l_currentTop, T_WINDOW_WIDTH / 2 - l_numOfCharsInLine / 2);
                break;
                
            default:                                                        /* Center alignment */
                
                _settextposition(l_currentTop, T_WINDOW_WIDTH / 2 - strlen(l_currentLine) / 2 + 1) ;
                break;
            } 
           _outtext(l_currentLine);
            l_currentTop++;
        }
        _outtext("\n");
    }
}
#endif

/*-----------------------------------------------------------------------------------------------*/
/* Function: StartingFlashConfiguration                                                          */
/*                                                                                               */
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:                                                                                  */
/*               Called by FUCore library when starting to configurate the flash access          */
/*-----------------------------------------------------------------------------------------------*/

void StartingFlashConfiguration()
{       
  
}

#ifdef __LINUX__
void EnterGraphicMode(short p_videoMode){}
void ExitGraphicMode(){}
#else
/*-----------------------------------------------------------------------------------------------*/
/* Function: EnterGraphicMode                                                                    */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Change the screen display to graphic mode  (640 x 480 16 colors)                */
/*-----------------------------------------------------------------------------------------------*/

void EnterGraphicMode(short p_videoMode)
{
    /* when the video mode is _UNKNOWN - stay in GRAPHIC_MODE_OFF and don't set the video mode */
    if(_UNKNOWN != p_videoMode)
    {
        _setvideomode(p_videoMode);
        _clearscreen(_GCLEARSCREEN);
        m_isScreenInGraphicMode = GRAPHIC_MODE_ON;
    }
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: ExitGraphicMode                                                                     */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Change the screen to text mode                                                  */
/*-----------------------------------------------------------------------------------------------*/

void ExitGraphicMode()
{
    if (GRAPHIC_MODE_OFF != m_isScreenInGraphicMode)
    {
        _setvideomode( _DEFAULTMODE );
        m_isScreenInGraphicMode = GRAPHIC_MODE_OFF;
    }
}

#endif

#ifdef __LINUX__
void FillProgressBar(unsigned int p_precent)
{
    
    char l_progBar[SIZE_OF_PROGRESS_BAR];
       
    SetProgressBar(p_precent, l_progBar);
    printf("%cPercentage completed: %s %d%% ",0x0D, l_progBar, p_precent);
    if(100 == p_precent)
    {
        printf("\n");
    }
    fflush(stdout);
    return;
}

#else
/*-----------------------------------------------------------------------------------------------*/
/* Function: FillProgressBar                                                                     */
/*                                                                                               */ 
/* Parameters:   p_precent - Precent to fill                                                     */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Fill the progress bar the the give precent                                      */
/*-----------------------------------------------------------------------------------------------*/

void FillProgressBar(unsigned int p_precent)
{
    
    int l_barWidth = 344;
    int l_barHeight = 20;
    int l_barLeft = G_WINDOW_WIDTH / 2 - 192;
    int l_barTop  = G_WINDOW_HEIGHT / 2 + 29;
    int l_fillWidth = (((double)(l_barWidth)) / 100) * p_precent;
    double l_tgWidthRatio = (double)T_WINDOW_WIDTH / (double)G_WINDOW_WIDTH;
    double l_tgHeightRatio = (double)T_WINDOW_HEIGHT / (double)G_WINDOW_HEIGHT;
    char l_buf[10];
    char l_progBar[SIZE_OF_PROGRESS_BAR];
       
    if (GRAPHIC_MODE_OFF == m_isScreenInGraphicMode)
    {
        SetProgressBar(p_precent, l_progBar);
        printf("%cPercentage completed: %s %d%% ",0x0D, l_progBar, p_precent);
        if(100 == p_precent)
        {
            printf("\n");
        }
        fflush(stdout);
        return;
    }
    
    _setcolor(PROGRESS_BAR_BACK_COLOR);
    _rectangle(_GFILLINTERIOR,
               l_barLeft,
               l_barTop,
               l_barLeft + l_barWidth,
               l_barTop + l_barHeight);

    _setcolor(PROGRESS_BAR_BORDER_COLOR);
    _rectangle(_GBORDER,
                l_barLeft,
                l_barTop ,
                l_barLeft + l_barWidth,
                l_barTop + l_barHeight);

    if (p_precent > 0)
    {
        if (l_fillWidth > 3)
        {
            _setcolor(PROGRESS_BAR_FILL_COLOR);
            _rectangle(_GFILLINTERIOR,
                       l_barLeft + 3 ,
                       l_barTop + 3,
                       l_barLeft + l_fillWidth - 3,
                       l_barTop + l_barHeight - 3);
        }
     
        _settextposition(18, 61);
        _setbkcolor(T_BLUE);
        _settextcolor(G_BRIGHT_WHITE);
        itoa(p_precent, l_buf, 10);
        _outtext(l_buf);
        _outtext("%");
    }
    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: drawWindow                                                                          */
/*                                                                                               */ 
/* Parameters:   p_left           - Window left position                                         */
/*               p_top            - Window top position                                          */
/*               p_width          - Window width                                                 */
/*               p_height         - Window height                                                */
/*               p_backColor      - Window back color                                            */
/*               p_textColor      - Window text color                                            */
/*               p_textBackColor  - Window text back color                                       */
/*               p_title          - Window top title                                             */
/*               p_bottomTitle    - Window bottom title                                          */
/*               p_titleBackColor - The title back color                                         */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Log the given text                                                              */
/*-----------------------------------------------------------------------------------------------*/

static void drawWindow(int p_left,
                       int p_top,
                       int p_width,
                       int p_height,
                       short p_backColor,
                       short p_textColor,
                       long p_textBackColor,
                       char *p_title,
                       char *p_bottomTitle,
                       short p_titleBackColor)
{

    int l_numOfCharsInTitle = 0;
    int l_titleTop          = 0;
    double l_tgHeightRatio  = (double)T_WINDOW_HEIGHT / (double)G_WINDOW_HEIGHT;
    int i, j;



    _setcolor(p_backColor);
    _rectangle(_GFILLINTERIOR, p_left, p_top, p_left + p_width, p_top + p_height);

    /* Drawing the title background */

    if (p_titleBackColor > 0)
    {
        _setcolor(p_titleBackColor);
        _rectangle(_GFILLINTERIOR, p_left, p_top, p_left + p_width, p_top + 16);
    }
    

    _setcolor(p_titleBackColor);
    _rectangle(_GBORDER,
                p_left + G_WINDOW_FRAME_OFFSET,
                p_top + G_WINDOW_FRAME_OFFSET,
                p_left + p_width - G_WINDOW_FRAME_OFFSET - 1,
                p_top + p_height - G_WINDOW_FRAME_OFFSET - 1);

     _rectangle(_GBORDER,
                p_left + G_WINDOW_FRAME_OFFSET + G_WINDOW_FRAME_SPACE,
                p_top + G_WINDOW_FRAME_OFFSET + G_WINDOW_FRAME_SPACE,
                p_left + p_width - G_WINDOW_FRAME_OFFSET - G_WINDOW_FRAME_SPACE - 1,
                p_top + p_height - G_WINDOW_FRAME_OFFSET - G_WINDOW_FRAME_SPACE- 1);


   
    l_titleTop = (p_top * l_tgHeightRatio);

    if (NULL != p_title)
    {
        _settextposition(l_titleTop , T_WINDOW_WIDTH / 2 - (strlen(p_title) + 2)/ 2) ;
        _setbkcolor(p_textBackColor);
        _settextcolor(p_textColor);
        _outtext(" ");
        _outtext(p_title);
        _outtext(" ");
    }

    l_numOfCharsInTitle =  strlen(p_bottomTitle) + 2;
    l_titleTop = T_WINDOW_HEIGHT ;

    if (NULL != p_bottomTitle)
    {
        _settextposition(l_titleTop , T_WINDOW_WIDTH / 2 - (strlen(p_bottomTitle) + 2)/ 2) ;
        _setbkcolor(p_textBackColor);
        _settextcolor(p_textColor);
        if (*p_bottomTitle != '\0')
        {
            _outtext(" ");
            _outtext(p_bottomTitle);
            _outtext(" ");
        } 
    }

    /* Drawing a line under the top title */
    
    if (p_titleBackColor > 0)
    {
        _setcolor(G_GRAY);
        _moveto(0, p_top + 16);
        _lineto(p_left + p_width, p_top + 16);
    }
}

#endif

/*-----------------------------------------------------------------------------------------------*/
/* Function: UpdateProgress                                                                      */
/*                                                                                               */
/* Parameters:   p_precent    - Will fill the progress bar to this value                         */
/*                                                                                               */
/* Returns:      APP_RESULT                                                                      */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:  Called by FUCore library while downloading to the flash                         */
/*-----------------------------------------------------------------------------------------------*/

void UpdateProgress(int p_precent)
{

    /* Verifying precent integrity */

    if (p_precent > 100)
    {
        p_precent = 100;
    }

    if (p_precent < 0)
    {
        p_precent = 0;
    }

    FillProgressBar(p_precent);
    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: mergeText                                                                           */
/*                                                                                               */
/* Parameters:   p_text       - Text with parameters seperated by ","                            */
/*                              Example: mergeText("Hello %s", "Israel");                        */
/*                                                                                               */
/* Returns:      APP_RESULT                                                                      */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:  Merge the given text with the given parameters. Be sure that p_text buffer      */
/*               is big enough to hold the entire text string                                    */
/*-----------------------------------------------------------------------------------------------*/

static void mergeText(char *p_text,...)
{
    char l_textBuffer[256];
    va_list l_argptr;
    

    
    va_start(l_argptr, p_text);
    vsprintf(l_textBuffer, p_text, l_argptr);
    va_end(l_argptr);

    strcpy(p_text, l_textBuffer);
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: AppExit                                                                             */
/*                                                                                               */
/* Parameters:   p_errorCode      - Error code                                                   */
/*               p_errorType      - FU error or App error                                        */
/*               p_additionalText - Parameters that will fit into the error text                 */
/*                                                                                               */
/* Returns:      APP_RESULT                                                                      */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:  Exit the program with/without an error message                                  */
/*-----------------------------------------------------------------------------------------------*/

void AppExit(int p_errorCode, ERROR_CODE_TYPE p_errorType, char *p_additionalText, ...)
{
    char *l_msgBoxTitle = NULL;
    char *l_msgBoxText  = NULL;
    int   i             = 0;
    int   l_lineLen     = 0;
    char  l_showErrMsg  = 0;
    

    
    if ((APP_RESULT_TYPE == p_errorType) && (APP_RESULT_OK != p_errorCode))
    {   
         l_showErrMsg = 1;
        l_msgBoxTitle = GetMsgBoxTitleString(p_errorCode);            
        if ((char)0 == l_msgBoxTitle[0])
        {
            l_msgBoxTitle = GetMsgBoxTitleString(APP_ERR_UNEXPECTED_ERROR);               
        }

        l_msgBoxText  =  GetAppTextString(p_errorCode);
        if ((char)0 == l_msgBoxText[0])
        {
            l_msgBoxText  = GetAppTextString(APP_ERR_UNEXPECTED_ERROR);
        }
        else
        {
            mergeText(l_msgBoxText, p_additionalText);
        }
    }
    else if((FU_RESULT_TYPE == p_errorType))
    {
        l_showErrMsg =1;
        if (REPORT_ONLY_APP_ERRORS == m_errorReportLevel)
        {                 
            l_msgBoxTitle = GetMsgBoxTitleString(APP_ERR_UNEXPECTED_ERROR);
            l_msgBoxText = GetAppTextString(APP_ERR_UNEXPECTED_ERROR);
        }
        else
        {
            l_msgBoxTitle = GetMsgBoxTitleString(FU_CORE_ERROR_OCCURRED);
            l_msgBoxText = GetFUErrorString(p_errorCode);
            if ((char)0 == l_msgBoxText[0])
            {
                l_msgBoxText = GetFUErrorString(FU_ERR_UNEXPECTED_ERROR);
            }
            else
            {
                mergeText(l_msgBoxText, p_additionalText);
            }
        }
    }

    if (l_showErrMsg)
    {    
        if (GRAPHIC_MODE_OFF == m_isScreenInGraphicMode)
        {   
            printf("\n%s\n", m_mainWindowTopTitle);
            l_lineLen = strlen(m_mainWindowTopTitle) + 2;
            for (i = 0; i < l_lineLen; i++)
            {
                printf("%c", '=');
            } 
            printf("\n\n%s\n", l_msgBoxTitle);
            l_lineLen = strlen(l_msgBoxTitle) + 2;
            for (i = 0; i < l_lineLen; i++)
            {
                printf("%c", '-');
            }        
            printf("\n %s\n", l_msgBoxText);        
        }
        else
        {
            DisplayCenterMsgBox(MSG_BOX_ERR, l_msgBoxTitle, 0, 1);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, l_msgBoxText);
            getch();
        }
    }
    
    /* Free the occupied memory */
#ifndef __LINUX__    
    if (NULL != m_companyLogoBmpFile)
    {
        free(m_companyLogoBmpFile->Header);
        free(m_companyLogoBmpFile->PixelData);
    }

    if (NULL != m_OEMLogoBmpFile)
    {
        free(m_OEMLogoBmpFile->Header);
        free(m_OEMLogoBmpFile->PixelData);
    }
#endif
    ExitGraphicMode();                      /* Restoring the user display */

    
    if(GetExitCodeType() == EXIT_CODE_BINARY && p_errorCode > 1)
    {
        p_errorCode = 1;
    }
    exit(p_errorCode);
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: PrintText                                                                           */
/*                                                                                               */
/* Parameters:   p_printType     - Error code as defined above                                   */
/*               p_textToPrint   - Error text to be printed                                      */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:                                                                                  */
/*               Print the given text                                                            */
/*-----------------------------------------------------------------------------------------------*/

void PrintText (int p_printType, char *p_textToPrint, ...)
{

    char l_buffer[256];    
    va_list l_argptr;

     

    /*------------------------------------------------------------------------------------------*/
    /*      Copy the arguments into the buffer for future printing                              */
    /*------------------------------------------------------------------------------------------*/
    va_start(l_argptr, p_textToPrint);
    vsprintf(l_buffer, p_textToPrint, l_argptr);
    va_end(l_argptr);


    switch (m_logLevel)
    {
        case 1:
            if (APP_VERBOSE_PRINT == p_printType)
            {
                addToLogFile(l_buffer);
            }
            break;
            
        case 2:
            addToLogFile(l_buffer);
            break;           
    }
    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: addToLogFile                                                                        */
/*                                                                                               */
/* Parameters:   p_logText        - Text to log                                                  */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:                                                                                  */
/*               Log the given text                                                              */
/*-----------------------------------------------------------------------------------------------*/

static void addToLogFile(char *p_logText)
{
    FILE *l_logFileHandle = NULL;


    
    /* Ignore lines that contains less then 6 (4 + newline) characters */

    if (strlen(p_logText) < 7)
    {
        return;
    }

 

    if (0 == m_logCounter)
    {
        /* This is the first line added to the log file. */
        /* We need to create the log file                */        
        
        l_logFileHandle = fopen(APP_DEFAULT_LOG_FILENAME, "wb");
    }
    else
    {
        /* The log file for the current run is already exist */

        l_logFileHandle = fopen(APP_DEFAULT_LOG_FILENAME, "ab+");
    }

    if (l_logFileHandle == NULL)
    {
        /* Error opening log file */  
        
        return;
    }
    
    m_logCounter++;


    fwrite(p_logText, 1, strlen(p_logText), l_logFileHandle);    
   
    fclose(l_logFileHandle);

}


/*-----------------------------------------------------------------------------------------------*/
/* Function: SetLogLevel                                                                         */
/*                                                                                               */ 
/* Parameters:   p_createFlag - 1: Log only verbose prints                                       */
/*                              2: Log all prints                                                */
/*                              else disable log                                                 */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: Set m_logLevel to be p_level                                                    */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Set the application log level. Indicates the type of prints to log              */
/*-----------------------------------------------------------------------------------------------*/

void SetLogLevel(int p_level)
{
    m_logLevel = p_level;
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: SetMainWindowTopTitle                                                               */
/*                                                                                               */
/* Parameters:   p_title     - The top title                                                     */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: Set m_mainWindowTopTitle to be the given title                                  */
/*                                                                                               */
/* Description:                                                                                  */
/*               Store the given title as the top title for future use                           */
/*-----------------------------------------------------------------------------------------------*/

void SetMainWindowTopTitle(char *p_title)
{
    strcpy(m_mainWindowTopTitle, p_title);
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: SetMainWindowBottomTitle                                                            */
/*                                                                                               */
/* Parameters:   p_title     - The bottom title                                                  */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */
/* Side effects: Set m_mainWindowBottomTitle to be the given title                               */
/*                                                                                               */
/* Description:                                                                                  */
/*               Store the given title as the bottom title for future use                        */
/*-----------------------------------------------------------------------------------------------*/

void SetMainWindowBottomTitle(char *p_title)
{
    strcpy(m_mainWindowBottomTitle, p_title);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        printTextMode                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_str -  string to print                                                               */
/*                                                                                                         */
/* Returns:         TRUE on success, FALSE on failure                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Prints the given string to screen.                                                     */
/*                  Each line contains maximum MAX_CHARS_PER_LINE chars                                    */
/*---------------------------------------------------------------------------------------------------------*/
static BOOLEAN printTextMode(char *p_str)
{
    int l_strLen           = strlen(p_str);
    char *l_strEnd         = p_str + (l_strLen-1);
    char *l_subStrStartPtr = p_str;
    char *l_subStrEndPtr   = p_str;
    char buf[MAX_CHARS_PER_LINE + 1];
    int l_subStrFullLen = 0, l_subStrTruncLen = 0;
    BOOLEAN l_retVal       = TRUE;
        
    /* go through the entire string */ 
    while(l_subStrStartPtr < l_strEnd)
    {
        /* calcualte the end of the sub string == max (l_strEnd, MAX_CHARS_PER_LINE-1) */ 
        if(l_subStrStartPtr + (MAX_CHARS_PER_LINE-1) > l_strEnd)
        {
            l_subStrEndPtr = l_strEnd;
            l_subStrFullLen = (l_subStrEndPtr - l_subStrStartPtr) + 1;
            /* only the tail left, so print is as a whole - no need for trancation */
            l_subStrTruncLen = l_subStrFullLen; 
        }
        else
        {
            l_subStrEndPtr = l_subStrStartPtr + (MAX_CHARS_PER_LINE-1);
            l_subStrFullLen = MAX_CHARS_PER_LINE;
            /* find the last space in the substring so we won't chop the last word */ 
            while((' ' != *l_subStrEndPtr) && (l_subStrStartPtr < l_subStrEndPtr))
            {
                l_subStrEndPtr--;
            }   
            l_subStrTruncLen = (l_subStrEndPtr - l_subStrStartPtr) + 1;
        }                        

        /* print the sub string */
        if(l_subStrStartPtr < l_subStrEndPtr)
        {
            /* print the sub string without the last word (or part of it) */
            //l_subStrTruncLen = (l_subStrEndPtr - l_subStrStartPtr) + 1;
            memcpy(buf, l_subStrStartPtr, l_subStrTruncLen);
            buf[l_subStrTruncLen] = '\0';                
            printf("%s\n", buf);
            /* ignore successive spaces for the next iteration */
            while((' ' == *l_subStrEndPtr) && (l_subStrEndPtr < l_strEnd))
            {
                l_subStrEndPtr++;
            }
            l_subStrStartPtr = l_subStrEndPtr;
        }
        else if(l_subStrStartPtr == l_subStrEndPtr)
        {
            /* the sub string has no spaces - print the whole length */
            memcpy(buf, l_subStrStartPtr, l_subStrFullLen);
            buf[l_subStrFullLen] = '\0';
            printf("%s\n", buf);
            l_subStrStartPtr += (l_subStrFullLen - 1);
        }
        else
        {
            /* we should never get to l_subStrStartPtr > l_subStrEndPtr */
            l_retVal = FALSE;
            break;
        }
    }    

    return l_retVal;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GetGraphicMode                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  None                                                                                   */
/*                                                                                                         */
/* Returns:         m_isScreenInGraphicMode                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the current graphic mode settings                                              */
/*---------------------------------------------------------------------------------------------------------*/
int GetGraphicMode()
{
    return m_isScreenInGraphicMode;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SetProgressBar                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_percent: percentage of the completed work                                            */
/*                  p_progBar: the progress bar to be set                                                  */
/*                                                                                                         */
/* Returns:         None                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Sets the given progress bar according to the given percentage                          */
/*---------------------------------------------------------------------------------------------------------*/
static void SetProgressBar(unsigned int p_percent, char* p_progBar)
{
    int i = 0;
    int l_numOfFillers = 0;

    /* Set the fixed chars and terminating null*/
    p_progBar[0] = '[';
    p_progBar[SIZE_OF_PROGRESS_BAR-2] = ']';
    p_progBar[SIZE_OF_PROGRESS_BAR-1] = '\0';

    /* Set the chars which indicate "not completed" */
    memset(&p_progBar[1],' ',(SIZE_OF_PROGRESS_BAR-3));
    
    if(p_percent > 100)
    {
        p_percent = 100;
    }
    
    l_numOfFillers = p_percent/10;

    /* Set the chars which indicate "completed" */
    for(i = 0; i < l_numOfFillers && (i+1) < (SIZE_OF_PROGRESS_BAR-2);i++)
    {
        p_progBar[i+1] = '.';
    }
            
}

