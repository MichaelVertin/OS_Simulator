/*
Provided by Dr. Michael Leverington NAU CS480 Fall2023
*/

// protect from multiple compiling
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "StandardConstants.h"

// GLOBAL CONSTANTS - may be used in other files
typedef enum { 
               SUBSTRING_NOT_FOUND = -1, 
               NON_PRINTABLE_CHAR = -2
             } StringUtilConstants;

/*
Name: compareString
Process: compares two strings with the following results:  
         if left string less than right string, returns less than zero
         if left string greater than right string, returns greater than zero
         if left string equals right string, returns zero
          - equals test includes length
Function Input/Parameters: c-style left and right strings (char *)
Function Output/Parameters: none
Function Output/Returned: result as specified (int)
Device Input/device: none
Device Output/device: none
Dependencies: getStringLength
*/
int compareString( const char *oneStr, const char *otherStr );



/*
Name: concatenateString
Process: appends one string onto another
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: getStringLength
*/
void concatenateString( char *destStr, const char *sourceStr );


/*
Name: copyString
Process: copies one string into another, 
         overwriting data in the destination string
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: getStringLength
*/
void copyString( char *destStr, const char *sourceStr );



/*
Name: findSubString
Process: linear search for given substring within another string
Function Input/Parameters: c-style source test string (char *), 
                           c-style source search string (char *)
Function Output/Parameters: none
Function Output/Returned: index fo found substring, or 
                          SUBSTRING_NOT_FOUND constant
                          if string not found
Device Input/device: none
Device Output/device: none
Dependencies: getStringLength
*/
int findSubString( const char *testStr, const char *searchSubStr );



/*
Name: getStringConstrained
Process: captures a string from the input stream
         with various constants
Function Input/Parameters: input stream (FILE *)
                           clears leading non printable (bool), 
                           clears leading space (bool), 
                           stops at non printable (bool), 
                           stops at specified delimiter (char)
                           Note: consumes delimiter
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/device: none
Device Output/device: none
Dependencies: fgetc
*/
bool getStringConstrained( 
                           FILE *inStream, 
                           bool clearLeadingNonPrintable, 
                           bool clearLeadingSpace, 
                           bool stopAtNonPrintable, 
                           char delimiter, 
                           char *capturedString
                         );



/*
Name: getStringLength
Process: finds the length of a string
         by counting characters up to the NULL_CHAR character
Function Input/Parameters: c-style string (char *)
Function Output/Parameters: none
Function Output/Returned: length of string
Device Input/device: none
Device Output/device: none
Dependencies: none
*/
int getStringLength( const char *testStr );


/*
Name: getStringToDelimiter
Process: captures a string from the input stream
         to a specified delimiter; 
         Note: consumes delimiter
Function Input/Parameters: input stream (FILE *)
                           stops at specified delimiter (char), 
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/device: none
Device Output/device: none
Dependencies: getStringConstrained( 
*/
bool getStringToDelimiter( 
                           FILE *inStream, 
                           char delimiter, 
                           char *capturedString
                         );


/*
Name: getSubString
Process: captures sub string within larger string
         between two inclusive indices. 
         returns empty string if either index is out of range, 
         assumes enough memory in destination string
Function Input/Parameters: c-style source string (char *), 
                           start and end indices (int)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: getStringLength, malloc, copyString, free
*/
void getSubString( char *destStr, const char *sourceStr, 
                        int startIndex, int endIndex );



/*
Name: setStrToLowerCase
Process: iterates through string, sets any upper case letter 
         to lower case; no effect on other letters
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: toLowerCase
*/
void setStrToLowerCase( char *destStr, const char *sourceStr );



/*
Name: toLowerCase
Process: if character is upper case, sets it to lower case;
         otherwise returns character unchanged
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Returned: character to set to lower case, if appropriate
Device Input/device: none
Device Output/device: none
Dependencies: none
*/
char toLowerCase( char testChar );






#endif // STRING_UTILS_H








