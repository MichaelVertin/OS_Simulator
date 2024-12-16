/*
Provided by Dr. Michael Leverington NAU CS480 Fall2023
*/

#include "metadataops.h"

/*
Name: addNode
Process: adds metadata node to linked list recursively, 
         handles empty list condition
Function Input/Parameters: pointer to head or next linked node (OpCodeType *), 
                           pointer to new node (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: pointer to previous node, or head node (OpCodeType *)
Device Input/Device: none
Device Output/Device: none
Dependencies: malloc, copyString
*/
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode )
{
    // check for local pointer assigned to NULL
    if( localPtr == NULL )
    {
        // access memory for new link/node
            // function: malloc
        localPtr = (OpCodeType *)malloc( sizeof( OpCodeType ) );

        // assign all three values to newly created node
        // assign next pointer to NULL
            // fucntion: copyString
        localPtr->pid = newNode->pid;
        copyString( localPtr->command, newNode->command );
        copyString( localPtr->inOutArg, newNode->inOutArg );
        copyString( localPtr->strArg1, newNode->strArg1 );
        localPtr->intArg2 = newNode->intArg2;
        localPtr->intArg3 = newNode->intArg3;
        localPtr->opEndTime = newNode->opEndTime;

        localPtr->nextNode = NULL;
        
        // return current local pointer
        return localPtr;
    }

    // assume end of list not found yet
    // assigned recursive function to current's next link
        // function: addNode
    localPtr->nextNode = addNode( localPtr->nextNode, newNode );

    // return current local pointer
    return localPtr;
}



/*
Name: clearMetaDataList
Process: recursively traverses list, frees dynamically allocated nodes
Function Input/Parameters: node op code (const OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: NULL (OpCodeType *)
Device Input/Device: none
Device Output/Device: none
Dependencies: clearMetaDataList, free
*/
OpCodeType *clearMetaDataList( OpCodeType *localPtr )
{
    // check for local pointer not set to null (list not empty)
    if( localPtr != NULL )
    {
        // call recursive function with next pointer
            // function: clearMetaDataList
        clearMetaDataList( localPtr->nextNode );

        // after recursive call, release memory to OS
            // function: free
        free( localPtr );

        // set given pointer to NULL
        localPtr = NULL;        
    }

    // return NULL to calling function
    return NULL;
}


/*
Name: displayMetaData
Process: data dump/display of all op code items
Function Input/Parameters: pointer to head
                           of op code/metadata list (const OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: printf, compareString, 
*/
void displayMetaData( const OpCodeType *localPtr )
{
    char outputStr[ MAX_STR_LEN ];

    // display title with underline
        // fucntion: printf
    printf( "Meta-Data File Display\n" );
    printf( "----------------------\n\n" );

    // loop to end of linked list
    while( localPtr != NULL )
    {
        // print leader
            // function: printf
        printf( "Op Code: " );

        // print op code pid
            // function: printf
        printf( "/pid: %d", localPtr->pid );

        // print op code command
            // function: printf
        printf( ".cmd: %s", localPtr->command );

        // check for dev op
        if( compareString( localPtr->command, "dev" ) == 0 )
        {
            // print in/out parameter
                // fucntion: printf
            printf( "/io: %s", localPtr->inOutArg );
        }

        // otherwise assume other than dev
        else
        {
            // print NA
                // fucntion: printf
            printf( "/io: NA" );
        }

        // print first string argument
            // fucntion: printf
        printf( "\n\t /arg1: %s", localPtr->strArg1 );

        // print first int argument
            // fucntion: printf
        printf( "/arg 2: %d", localPtr->intArg2 );

        // print second int argument
            // function: printf
        printf( outputStr, "/arg 3: %d", localPtr->intArg3 );

        // print op end time
        printf( "/op end time: %8.6f", localPtr->opEndTime );

        // end line
            // function: printf
        printf( "\n\n" );

        // assign local pointer to next node
        localPtr = localPtr->nextNode;
    }
}


/*
Name: getCommand
Process: parses three letter command part of op code string
Function Input/Parameters: input of code string (const char *), 
                           starting index (int)
Function Output/Parameters: parsed command (char *)
Function Output/Returned: updated starting index for use
                          by calling function (int)
Device Input/Device: none
Device Output/Device: none
Dependencies: none
*/
int getCommand( char *cmd, const char *inputStr, int index )
{
    // initialize variable
    int lengthOfCommand = 3;

    // loop across command length
    while( index < lengthOfCommand )
    {
        // assign character from input string to buffer string
        cmd[ index ] = inputStr[ index ];

        // increment index
        index++;
 
        // set next character to NULL_CHAR
        cmd[ index ] = NULL_CHAR;
    }

    // return current index
    return index;
}



/*
Name: getMetaData
Process: main driver function to upload, parse, and store list         
         of op code commands in a linked list
Function Input/Parameters: file name (const char *)
Function Output/Parameters: pointer
                            to op code linked list head pointer (OpCodeType **), 
                            result message of function state
                            after completion (char *)
Function Output/Returned: Boolean resultof operation (bool)
Device Input/file: op code list uploaded
Device Output/device: none
Dependencies: copyString, fopen, getStringToDelimiter, compareString, fclose, 
              malloc, getOpCommand, updateStartCount, updateEndCount, 
              clearMetaDataList, free, addNode
*/
bool getMetaData( const char *fileName, 
                                 OpCodeType **opCodeDataHead, char *endStateMsg )
{
    // initilaize function/variables

        // initialize read only constant
        const char READ_ONLY_FLAG[] = "r";

        // initialize variables
        int accessResult, startCount = 0, endCount = 0;
        char dataBuffer[ MAX_STR_LEN ];
        bool returnState = true;
        OpCodeType *newNodePtr;
        OpCodeType *localHeadPtr = NULL;
        FILE *fileAccessPtr;

    // intialize op code data pointer in case of return error
    *opCodeDataHead = NULL;

    // initialize ned state message 
        // function: copyString
    copyString( endStateMsg, "Metadata file upload successful" );

    // open file for reading
        // function: fopen
    fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

    // check for file open failure
    if( fileAccessPtr == NULL )
    {
        // set end state message
            // fucntion: copyString
        copyString( endStateMsg, "Metadata file access error" );

        // return file access error
        return false;
    }

    // check first line for correct leader
        // function: getStringToDelimiter, compareString
    if( !getStringToDelimiter( fileAccessPtr, COLON, dataBuffer )
            || compareString( dataBuffer, 
                                    "Start Program Meta-Data Code" ) != 0 )
    {
        // close file
            // fucntion: fclose
        fclose( fileAccessPtr );

        // set end state message
            // fucntion: copyString
        copyString( endStateMsg, "Corrupt metadata leader line error" );

        // return corrupt descriptor error
        return false;
    }

    // allocate memroy for temporary data structure
        // fucntion: malloc
    newNodePtr = ( OpCodeType * ) malloc( sizeof( OpCodeType ) );

    // get the first op command
        // fucntion: getOpCommand
    accessResult = getOpCommand( fileAccessPtr, newNodePtr );

    // get start and end counts for later comparison
        // fucntion: updateStartCount, updateEndCount
    startCount = updateStartCount( startCount, newNodePtr->strArg1 );
    endCount = updateEndCount( endCount, newNodePtr->strArg1 );

    // check for failure of first complete op command
    if( accessResult != COMPLETE_OPCMD_FOUND_MSG )
    {
        // close file
            // function: fclose
        fclose( fileAccessPtr );

        // clear data from the structure list
            // fucntion: clearMetaDataList
        *opCodeDataHead = clearMetaDataList( localHeadPtr );

        // free temporary structure memory
            // function: free
        free( newNodePtr );

        // set end state message
            // function: copyString
        copyString( endStateMsg, "Metadata incomplete first op command found" );

        // return result of operation
        return false;
    }

    // loop across all remaining op commands
    //   (while complete op commands are found)
    while( accessResult == COMPLETE_OPCMD_FOUND_MSG )
    {
         // add the new op command to the linked list
            // function: addNode
        localHeadPtr = addNode( localHeadPtr, newNodePtr );

        // get a new op command
            // fucntion: getOpCommand
        accessResult = getOpCommand( fileAccessPtr, newNodePtr );

        // update start and end counts for later comparison
            // function: updateStartCount, updateEndCount
        startCount = updateStartCount( startCount, newNodePtr->strArg1 );
        endCount = updateEndCount( endCount, newNodePtr->strArg1 );
    }

    // after loop completion, check for last op command found
    if( accessResult == LAST_OPCMD_FOUND_MSG )
    {
        // check for start and end op code counts equal
        if( startCount == endCount )
        {
            // add the last node to the linked list
                // fucntion: addNode
            localHeadPtr = addNode( localHeadPtr, newNodePtr );

            // set access result to no error for later operation
            accessResult = NO_ACCESS_ERR;

            // check last line for incorrect end descriptor
                // function: getStringToDdelimiter, compareString
            if( !getStringToDelimiter( fileAccessPtr, PERIOD, dataBuffer )
              || compareString( dataBuffer, "End Program Meta-Data Code" )
                                                                   != 0 )
            {
                // set access result to corrupted descriptor error
                accessResult = MD_CORRUPT_DESCRIPTOR_ERR;

                // set end state message
                    // function: copyString
                copyString( endStateMsg, "Metadata corrupted descriptor error" );           
            }
        }

        // otherwise, assume start/end counts not equal
        else
        {
            // close file
                // fucntion: fclose
            fclose( fileAccessPtr );

            // clear data from the structure list
                // function: clearMetaDataList
            *opCodeDataHead = clearMetaDataList( localHeadPtr );

            // free temporary structure memory
                // function: free
            free( newNodePtr );

            // set access result to error
            accessResult = UNBALANCED_STATE_END_ERR;

            // set end state message
                // fucntion: copyString
            copyString( endStateMsg, 
                             "Unbalanced start and end arguments in metadata" );

            // return result of operation
            return false;
        }
    }

    // otherwise, assume didn't find
    else
    {
        // set end state message
            // function: copyString
        copyString( endStateMsg, "Corrupted metadataop code" );

        // unset return state
        returnState = false;
    }

    // check for any errors found (not no error)
    if( accessResult != NO_ACCESS_ERR )
    {
        // clear the op command list
            // fucntion: clearMetaDataList
        localHeadPtr = clearMetaDataList( localHeadPtr );
    }

    // close access file
        // fucntion: fclose
    fclose( fileAccessPtr );

    // release temporary structure memory
        // function: free
    free( newNodePtr );

    // assign temporary local head pointer to parameter return pointer
    *opCodeDataHead = localHeadPtr;

    // return access result
    return returnState;
}








/*
Name: getOpCommand
Process: acquires one op command line from a previously opened file, 
         parses it, and sets various struct members according 
         to the three letter command
Function Input/Parameters: pointer to open file handle (FILE *)
Function Output/Parameters: pointer to one op code struct (OpCodeType *)
Function Output/Returned: coded result of operation (OpCodeMessages)
Device Input/Device: op code line uploaded
Device Output/Device: none
Dependencies: getStringToDelimiter, getCommand, copyString, verifyValidCommand, 
              compareString, getStringArg, verifyFirstStringArg, getNumberArg
*/
OpCodeMessages getOpCommand( FILE *filePtr, OpCodeType *inData )
{
    // initialize function/variables

        // initialize local components
        const int MAX_CMD_LENGTH = 5;
        const int MAX_ARG_STR_LENGTH = 15;

        // initialize other variables
        int numBuffer = 0;
        char strBuffer[ STD_STR_LEN ];
        char cmdBuffer[ MAX_CMD_LENGTH ]; 
        char argStrBuffer[ MAX_ARG_STR_LENGTH ];
        int runningStringIndex = 0;
        bool arg2FailureFlag = false;
        bool arg3FailureFlag = false;

    // get whole op command as string, check for successful access
        // function: getStringToDelimiter
    if( getStringToDelimiter( filePtr, SEMICOLON, strBuffer ) )
    {
        // get three-letter command
            // function: getCommand
        runningStringIndex = getCommand( cmdBuffer, 
                                               strBuffer, runningStringIndex );
 
        // assign op command to node
            // fucntion: copyString
        copyString( inData->command, cmdBuffer );
    }

    // otherwise, assume unsuccessful access
    else
    {
        // set pointer to data structure to null
        inData = NULL;

        // return op command access failure
        return OPCMD_ACCESS_ERR;
    }

    // verify op command
    if( !verifyValidCommand( cmdBuffer ) )
    {
        // return op command error
        return CORRUPT_OPCMD_ERR;
    }

     // set all struct values that may not be initialized to defaults
    inData->pid = 0;
    inData->inOutArg[ 0 ] = NULL_CHAR;
    inData->intArg2 = 0;
    inData->intArg3 = 0;
    inData->opEndTime = 0.0;
    inData->nextNode = NULL;

    // check for device command
    if( compareString( cmdBuffer, "dev" ) == 0 )
    {
        // get in/out argument
        runningStringIndex = getStringArg( argStrBuffer, 
                                              strBuffer, 
                                              runningStringIndex );

        // set device in/out argument
        copyString( inData->inOutArg, argStrBuffer );

        // check correct argument
        if( compareString( argStrBuffer, "in" ) != 0 
            && compareString( argStrBuffer, "out" ) != 0 )
        {
            // return argument error
            return CORRUPT_OPCMD_ARG_ERR;
        }
    }

    // get first string arg
    runningStringIndex = getStringArg( argStrBuffer, 
                                              strBuffer, runningStringIndex );

    // set device in/out argument
    copyString( inData->strArg1, argStrBuffer );

    // check for legitimate first string arg
    if( !verifyFirstStringArg( argStrBuffer ) )
    {
        // return argument error
        return CORRUPT_OPCMD_ARG_ERR;
    }

    // check for last op command found
    if( compareString( inData->command, "sys" ) == 0 
                          && compareString( inData->strArg1, "end" ) == 0 )
    {
        // return last op command found message
        return LAST_OPCMD_FOUND_MSG;
    }

    // check for app start seconds argument
    if( compareString( inData->command, "app" ) == 0 
                        && compareString( inData->strArg1, "start" ) == 0 )
    {
        // get number argument
            // fucntion: getNumberArg
        runningStringIndex = getNumberArg( &numBuffer, 
                                               strBuffer, runningStringIndex );

        // check for failed number access
        if( numBuffer <= BAD_ARG_VAL )
        {
            // set failure flag
            arg2FailureFlag = true;
        }

        // set first int argument to nubmer
        inData->intArg2 = numBuffer;
    }

    // check for cpu cycle time
    else if( compareString( inData->command, "cpu" ) == 0 )
    {
        // get number argument
            // function: getNumberArg
        runningStringIndex = getNumberArg( &numBuffer, 
                                               strBuffer, runningStringIndex );

        // check for failure number access
        if( numBuffer <= BAD_ARG_VAL )
        {
            // set failure flag
            arg2FailureFlag = true;
        }

        // set first int argument to number
        inData->intArg2 = numBuffer;
    }

    // check for device cycle time
    else if( compareString( inData->command, "dev" ) == 0 )
    {
        // get number argument
            // function: getNumberArg
        runningStringIndex = getNumberArg( &numBuffer, strBuffer, runningStringIndex );

        // check for failed number access
        if( numBuffer <= BAD_ARG_VAL )
        {
            // set failure flag
            arg2FailureFlag = true;
        }

        // set first int argument to number
        inData->intArg2 = numBuffer;
    }

    // check for memory base offset
    else if( compareString( inData->command, "mem" ) == 0 )
    {
        // get number argument for base
            // function: getNumberArg
        runningStringIndex = getNumberArg( &numBuffer, 
                                               strBuffer, runningStringIndex );

        // check for failed number access
        if( numBuffer <= BAD_ARG_VAL )
        {
            // set failure flag
            arg2FailureFlag = true;
        }

        // set first int argument to number
        inData->intArg2 = numBuffer;

        // get number argument for offset
            // function: getNumberArg
        runningStringIndex = getNumberArg( &numBuffer, 
                                               strBuffer, runningStringIndex );

        // check for failed access
        if( numBuffer <= BAD_ARG_VAL )
        {
            // set failure flag
            arg3FailureFlag = true;
        }

        // set second int argument to number
        inData->intArg3 = numBuffer;
    }

    // check int args for uploaded failure
    if( arg2FailureFlag || arg3FailureFlag )
    {
        // return corrupt op command error error
        return CORRUPT_OPCMD_ERR;
    }

    // return complete op command/found message
    return COMPLETE_OPCMD_FOUND_MSG;
}

/*
Name: getNumberArg
Process: starts at given index, captures and assembles integer argument, 
         and returns as parameter
Function Input/Parameters: input string (const char *), starting index (int)
Function Output/Parameters: pointer to captured integer value
Function Output/Returned: updated index for next function start
Device Input/Device: none
Device Output/Device: none
Dependencies: isDigit
*/
int getNumberArg( int *number, const char *inputStr, int index )
{
    // initalize function/variables
    bool foundDigit = false;
    *number = 0;
    int multiplier = 10;

    // loop to skip white space
    while( inputStr[ index ] <= SPACE || inputStr[ index ] == COMMA )
    {
        index++;
    }

    // loop across string length
    while( isDigit( inputStr[ index ] ) && inputStr[ index ] != NULL_CHAR )
    {
        // set digit found flag
        foundDigit = true;

        // assign digit to output
        *number = (*number) * multiplier + (int)( inputStr[ index ] - '0' );

        // increment index
        index++;
    }

    if( !foundDigit )
    {
        *number = BAD_ARG_VAL;
    }

    // return current index
    return index;
}


/*
Name: getStringArg
Process: starts at given index, captures and assembles string argument, 
         and returns as parameter
Function Input/Parameters: input string (const char *), starting index (int)
Function Output/Parameters: pointer to captured string argument (char *)
Function Output/Returned: updated index for next function start
Device Input/Device: none
Device Output/Device: none
Dependencies: none
*/
int getStringArg( char *strArg, const char *inputStr, int index )
{
    // initialize function/variables
    int localIndex = 0;

    // loop to skip white space
    while( inputStr[ index ] == SPACE || inputStr[ index ] == COMMA )
    {
        index++;
    }

    // loop across string length
    while( inputStr[ index ] != COMMA && inputStr[ index ] != NULL_CHAR )
    {
        // assign character from input string to buffer string
        strArg[ localIndex ] = inputStr[ index ];

        // increment index
        index++; localIndex++;

        // set next character to null character
        strArg[ localIndex ] = NULL_CHAR;
    }

    // return currentIndex
    return index;
}


/*
Name: isDigit
Process: tests character parameter for digit, returns true if is digit, 
         false otherwise
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/Device: none
Device Output/Device: none
Dependencies: none
*/
bool isDigit( char testChar )
{
    // check for test character between '0' and '9', return
    return testChar >= '0' && testChar <= '9';
}




/*
Name: updateEndCount
Process: manages cound of "end" arguments to be comared at end 
        of process input
Function Input/Parameters: initial count (int), 
                           test string for "end" (const char *)
Function Output/Parameters: none
Function Output/Returned: updated count, if "end" string found, 
                          otherwise no change
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
int updateEndCount( int count, const char *opString )
{
    // check for "end" is op string
        // function: compareString
    if( compareString( opString, "end" ) == 0 )
    {
        // return incremented end count
        return count + 1;
    }

    // return unchanged end count
    return count;
}


/*
Name: updateStartCount
Process: manages count of "start" arguments to be compared at end 
         of process input
Function Input/Parameters: initial count (int), 
                           test string for "start" (const char *)
Function Output/Parameters: none
Function Output/Returned: updated count, if "start" string found, 
                          otherwise no change
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
int updateStartCount( int count, const char *opString )
{
    // check for "start" in op string
        // function: compareString
    if( compareString( opString, "start" ) == 0 )
    {
        // return incremented start count
        return count + 1;
    }

    // return unchanced start count
    return count;
}


/*
Name: veryifyFirstStringArg
Process: checks for all possibilitiesof first argument string of op command
Function Input/Parameters: test string (const char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
bool verifyFirstStringArg( const char *strArg )
{
    // check for string holding correct first argument, return
        // fucntion: compareString
    return compareString( strArg, "access" ) == 0 
        || compareString( strArg, "allocate" ) == 0 
        || compareString( strArg, "end" ) == 0 
        || compareString( strArg, "ethernet" ) == 0 
        || compareString( strArg, "hard drive" ) == 0 
        || compareString( strArg, "keyboard" ) == 0 
        || compareString( strArg, "monitor" ) == 0 
        || compareString( strArg, "printer" ) == 0 
        || compareString( strArg, "process" ) == 0 
        || compareString( strArg, "serial" ) == 0 
        || compareString( strArg, "sound signal" ) == 0 
        || compareString( strArg, "start" ) == 0 
        || compareString( strArg, "usb" ) == 0 
        || compareString( strArg, "video signal" ) == 0;
}



/*
Name: verifyValidCommand
Process: checks for all possiblities of three-letter op code command
Function Input/Parameters: test string for command (const char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of test (bool)
Device Input/Device: none
Device Output/Device: none
Dependencies: compareString
*/
bool verifyValidCommand( char *testCmd )
{
    // check for holding three-letter op code command, return
        // function: compareString
    return compareString( testCmd, "sys" ) == 0 
        || compareString( testCmd, "app" ) == 0 
        || compareString( testCmd, "cpu" ) == 0 
        || compareString( testCmd, "mem" ) == 0 
        || compareString( testCmd, "dev" ) == 0;
}








