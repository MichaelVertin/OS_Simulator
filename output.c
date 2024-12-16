#include "output.h"


void output( char *toDisplay )
{
    char timeStr[ MIN_STR_LEN ];
    char outstr[ MAX_STR_LEN ];
    accessTimer( LAP_TIMER, timeStr );
    sprintf( outstr, "%10s, %s", timeStr, toDisplay );
    outputDirectly( outstr );
}

void outputDirectly( char *outStr )
{
    OutputFormatType *format;
    format = accessOutputFormat();
    if( format->consoleDisplay )
    {
        printf( "%s", outStr );
    }
    if( format->fileDisplay )
    {
        addStringToOutput( accessOutputFormat(), outStr );
    }
}

void outputNewline()
{
    outputDirectly( "\n" );
}

OutputFormatType *accessOutputFormat()
{
    static OutputFormatType outputData;

    return &outputData;
}


void setFileOutput( bool isSet )
{
    OutputFormatType *format;
    format = accessOutputFormat();

    format->fileDisplay = isSet;
    if( isSet )
    {
        // initialize the output array
        initializeOutputArr( format );
    }
}

void setConsoleOutput( bool isSet )
{
    OutputFormatType *format;
    format = accessOutputFormat();
    format->consoleDisplay = isSet;
}

void displayDataToFile( const char *filename )
{
    OutputFormatType *format = accessOutputFormat();
    FILE *filePtr;
    int strInd;

    if( format->fileDisplay )
    {
        filePtr = fopen( filename, "w" );

        // print all strings in stringArr to file
        for( strInd = 0; strInd < format->size; strInd++ )
        {
            fprintf( filePtr, "%s", format->stringArr[ strInd ] );
        }

        // destruct all memory
        fclose( filePtr );
        freeStringArray( format->stringArr, format->capacity );

        // set unable to display
        format->fileDisplay = false;
    }
}


void initializeOutputArr( OutputFormatType *format )
{
    format->size = 0;

    // allocate memory for one string
    format->capacity = 1;
    format->stringArr = (char **)malloc( sizeof( char * ) * format->capacity );
    format->stringArr[0] = (char *)malloc( sizeof( char *) * MAX_STR_LEN );
}

void addStringToOutput( OutputFormatType *format, const char *newString )
{
    char **newStringArr;
    int wkgStringIndex;
    int newCapacity;

    // realllocate memory if not enough capacity
    if( format->size >= format->capacity )
    {
        // double capacity
        newCapacity = format->capacity * 2;
        // allocate memory for newStringArr
        newStringArr = (char **)malloc( sizeof(char *) * newCapacity );

        // allocate memory through capacity
        for( wkgStringIndex = 0; 
                     wkgStringIndex < newCapacity; 
                             wkgStringIndex++ )
        {
            // allocate memory for new string
            newStringArr[ wkgStringIndex ] = (char *)malloc( 
                                                sizeof( char *) * MAX_STR_LEN );
        }

        // copy current data from format into newStringArr
        for( wkgStringIndex = 0; 
                     wkgStringIndex < format->size; 
                             wkgStringIndex++ )
        {
            // copy old string into new string
            copyString( newStringArr[ wkgStringIndex ], 
                                          format->stringArr[ wkgStringIndex ] );
        }

        // free old memory
        freeStringArray( format->stringArr, format->capacity );

        // set new values
        format->capacity = newCapacity;
        format->stringArr = newStringArr;
    }
    // done reallocating, assume sufficient memory

    // copy the newString at the end
    copyString( format->stringArr[ format->size ], newString );

    // increment the size
    format->size++;
}

void freeStringArray( char **stringArr, int capacity )
{
    int wkgStringIndex;

    // first free string memory
    for( wkgStringIndex = 0; 
                wkgStringIndex < capacity; 
                          wkgStringIndex++ )
    {
        free( stringArr[ wkgStringIndex ] );
    }

    // free string array
   free( stringArr );
}







