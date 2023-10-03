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



#include "ErrorCodes.h"

int fcloseall();

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             LOCAL FUNCTION DECLARATION                               */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* determines whether the exit code will be binary or every possible value                       */
/*-----------------------------------------------------------------------------------------------*/
static EXIT_CODE_TYPE m_exitCodeType = EXIT_CODE_ANY_VALUE; 

/*--------------------------------------------------------------------------------------*/
/* Function: printCustomMessage		                                                    */
/*                                                                                      */ 
/* Parameters:   p_errorNumber - Error code as defined above                            */
/*				 p_errorText   - Error text to be printed								*/
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:																			*/
/*				Prints a custom error message according to the supplied error code		*/
/*				and the error text														*/
/*--------------------------------------------------------------------------------------*/

static void printCustomMessage(int p_errorNumber, char *p_errorText);




/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             FUNCTION IMPLEMENTATION                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


void ErrorHandle(int p_errorNumber, int p_customPrint, char *p_errorText, ...)
{
	
	char l_buffer[256];    
	va_list l_argptr;

     
    
	if (p_errorText != NULL)
	{

/*--------------------------------------------------------------------------------------*/
/*      Copy the arguments into the buffer for future printing                          */
/*--------------------------------------------------------------------------------------*/
	
		va_start(l_argptr, p_errorText);
	    vsprintf(l_buffer, p_errorText, l_argptr);
	    va_end(l_argptr);		
	}


	switch (p_customPrint)
	{
	case 0:
	
/*--------------------------------------------------------------------------------------*/
/*      print a custom error message depanding on the error code                        */
/*--------------------------------------------------------------------------------------*/
		
        printf("\nError %d", p_errorNumber);

		printCustomMessage(p_errorNumber, l_buffer);


		break;


	case 1:
		
/*--------------------------------------------------------------------------------------*/
/*      Print the error code with the give error text                                   */
/*--------------------------------------------------------------------------------------*/

		printf("\nError %d", p_errorNumber);

		if (p_errorText != NULL)
		{
			printf(": ");
			printf("%s",l_buffer);
			printf("\n");
		}
		else
		{
			printf("\n");
		}       
		
		break;

	case 2:

/*--------------------------------------------------------------------------------------*/
/*      No printing at all                                                              */
/*--------------------------------------------------------------------------------------*/

		break;


	default:
		
		printf("\nError %d\n", p_errorNumber);

		break;
	}

/*--------------------------------------------------------------------------------------*/
/*  Negetive error number indicates a significative error                               */
/*--------------------------------------------------------------------------------------*/
 
	if (p_errorNumber < 0)
	{
        fcloseall();
        
        if(EXIT_CODE_BINARY == m_exitCodeType)
        {
            p_errorNumber= 1;
        }

		exit(p_errorNumber);
	}

}

static void printCustomMessage(int p_errorNumber, char *p_errorText)
{
	
	printf(": ");


	switch (p_errorNumber)
	{
	
	case ERROR_READING_FROM_FILE:
		printf("Reading from %s failed\n", p_errorText);
		break;
	
	case ERROR_WRITING_TO_FILE:
		printf("Writing to %s failed\n", p_errorText);
		break;

	case ERROR_OPENING_FILE:
		printf("Failed to open %s\n", p_errorText);
		break;
	
	case ERROR_FILE_MISSING:
		printf("%s missing\n", p_errorText);
		break;

	case ERROR_TARGET_DRIVE_IS_FULL:
		printf("There is not enough space on drive\n");
		break;

	case ERROR_COMPRESSING_FILE:
		printf("Failed to compress %s\n", p_errorText);
		break;

	case ERROR_MALLOC_FAILED:
		printf("Memory allocation failed %s\n", p_errorText);
		break;

	default:

		if (p_errorText != NULL)
		{
			printf("%s",p_errorText);
		}
		
		break;

	}

	printf("\n");

}

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

void SetExitCodeType(EXIT_CODE_TYPE p_exitCodeType)
{
    m_exitCodeType = p_exitCodeType;
}

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

EXIT_CODE_TYPE GetExitCodeType()
{
    return m_exitCodeType;
}


