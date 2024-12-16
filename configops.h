/*
Provided by Dr. Michael Leverington NAU CS480 Fall2023
*/

#ifndef CONFIGOPS_H
#define CONFIGOPS_H

#include "StandardConstants.h"
#include "StringUtils.h"
#include "datatypes.h"



/*
Name: clearConfigData
Process: free dynamically allocated config data structure
         if it has not already been freed
Function Input/Parameters: pointer to config data structure (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: NULL (ConfigDataType *)
Device Input/Device: none
Device Output/Device: none
Dependencies: tbd
*/
ConfigDataType *clearConfigData( ConfigDataType *configData );



/*
Name: configCodeToString
Process: utility function converts configuration code numbers
         to the string they represent
Function Input/Parameters: configuration code (int)
Function Output/Parameters: resulting output string (char *)
Function Output/Returned: none
Device Input/Device: none
Device Output/monitor: none
Dependencies: copyString  
*/
void configCodeToString( int code, char *outString );



/*
Name: displayConfigData
Process: screen dump/display of all config data
Function Input/Parameters: pointer to config data structure (ConfigDataType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: displayed as specified
Dependencies: tbd
*/
void displayConfigData( ConfigDataType *configData );


/*
Name: getConfigData
Process: driver function for capturing configuration data from a config file
Function Input/Parameters: file name (const char *)
Function Output/Parameters: pointer to config data pointer (ConfigDataType **), 
                            end/result state message pointer (char *)
Function Output/Returned: Boolean result of data access operation (bool)
Device Input/file: config data uploaded
Device Output/device: none
Dependencies: tbd
*/
bool getConfigData( const char *fileName, 
                    ConfigDataType **configData, char *endStateMsg );



/*
Name: getDataLineCode
Process: converts leader line string to configuration code value
         (all config file leader lines)
Function Input/Parameters: config leader line string (const char *)
Function Output/Parameters: none
Function Output/Returned: configuration code value (ConfigCodeMessages)
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
ConfigCodeMessages getDataLineCode( const char *dataBuffer );


/*
Name: getCpuSchedCode
Process: converts cpu schedule string to code (all scheduling possibilities)
Function Input/Parameters: lower case code string (const char *)
Function Output/Parameters: none
Function Output/Returned: cpu schedule code (ConfigDataCodes)
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
ConfigDataCodes getCpuSchedCode( const char *lowerCaseStr );


/*
Name: getLogToCode
Process: concerts "log to" text to configuration data code
         (three log to strings)
Function Input/Parameters: lower case log to string (const char *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: tbd
*/
ConfigDataCodes getLogToCode( const char *lowerCaseLogToStr );


/*
Name: stripTrailingSpaces
Process: removes trailing spaces from input config leader lines
Function Input/Parameters: config leader line string (char *)
Function Output/Parameters: updated config leader line string (char *)
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: getStringLength
*/
void stripTrailingSpaces( char *str );



/*
Name: valueInRange
Process: checks for config data values in range, including string values
         (all config data values)
Function Input/Parameters: line code number for specific config value (int), 
                           integer value, as needed (int), 
                           double value, as needed (double), 
                           string value, as needed (const char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of range test (bool)
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
bool valueInRange( int lineCode, int intVal, 
                             double doubleVal, const char *lowerCaseStringVal );






#endif // CONFIGOPS_H

