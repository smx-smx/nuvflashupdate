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


#ifndef ERRORCODES_HEADER 
#define ERRORCODES_HEADER


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                            EXTERNAL FUNCTION DECLARATION                             */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                               ERROR CODE DEFINITIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/ 

/* Program */

#define ERROR_ILLEGAL_TARGET_EXE_NAME           -11

#define ERROR_MALLOC_FAILED						-90

/* File */

#define ERROR_READING_FROM_FILE                 -101
#define ERROR_WRITING_TO_FILE                   -102
#define ERROR_OPENING_FILE                      -103
#define ERROR_FILE_MISSING                      -104
#define ERROR_TARGET_DRIVE_IS_FULL              -105
#define ERROR_HELP_FILE_MISSING                 -106
#define ERROR_MSG_FILE_MISSING                  -107



/* Compression */

#define ERROR_COMPRESSING_FILE                  -200

typedef enum
{
    
    EXIT_CODE_ANY_VALUE, /* exit code scheme: success - 0, failure - other */
    EXIT_CODE_BINARY     /* exit code scheme: success - 0, failure - 1 */
    
} EXIT_CODE_TYPE;



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                FUNCTION DECLERATIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------*/
/* Function: ErrorHandle                                                                */
/*                                                                                      */ 
/* Parameters:   p_errorNumber - Error code as defined above                            */
/*                                                                                      */
/*               p_customPrint - Select error print type                                */
/*							     0 -> Text depands on the error code                    */
/*							     1 -> Print the error code with error text              */
/*                                                                                      */
/*			     p_errorText   - Error text to be printed                               */
/*                                                                                      */
/* Returns:      None                                                                   */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Handle error event. In case of a negetive error number will exit       */
/*               the program                                                            */ 
/*--------------------------------------------------------------------------------------*/

void ErrorHandle(int p_errorNumber, int p_customPrint, char *p_errorText, ...);

/*-----------------------------------------------------------------------------------------------*/
/* Function: SetExitCodeType                                                                 */
/*                                                                                               */ 
/* Parameters:   p_exitCodeType                                                                  */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: Set m_exitCodeType to be p_exitCodeType                                         */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Sets the exit code type of the application - any value or binary                */
/*-----------------------------------------------------------------------------------------------*/

void SetExitCodeType(EXIT_CODE_TYPE p_exitCodeType);

/*-----------------------------------------------------------------------------------------------*/
/* Function: GetExitCodeType                                                                 */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Returns m_exitCodeType                                                          */
/*-----------------------------------------------------------------------------------------------*/

EXIT_CODE_TYPE GetExitCodeType();

#ifdef __cplusplus
}                                                       /* Closing brace for extern "C" */
#endif

#endif
