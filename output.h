#ifndef OUTPUT_H
#define OUTPUT_H

#include "simtimer.h"
#include "StringUtils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct OutputFormatStruct
{
    bool fileDisplay, consoleDisplay;
    char **stringArr;
    int size, capacity;
} OutputFormatType;




void output( char *toDisplay );

void outputDirectly( char *toDisplay );

void outputNewline();

OutputFormatType *accessOutputFormat();

void setFileOutput( bool isSet );

void setConsoleOutput( bool isSet );

void displayDataToFile( const char *filename );

void initializeOutputArr( OutputFormatType *format );

void addStringToOutput( OutputFormatType *format, const char *newString );

void freeStringArray( char **stringArr, int capacity );

#endif // OUTPUT_H
