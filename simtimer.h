// Program Information ////////////////////////////////////////////////////////
/**
 * @file SimpleTimer.h
 *
 * @brief Header file for micro-second precision timer
 * 
 * @author Michael Leverington
 * 
 * @details Specifies all member methods of the SimpleTimer
 *
 * @version 2.00 (13 January 2017)
 *          1.00 (11 September 2015)
 *
 * @Note None
 */

// Precompiler directives /////////////////////////////////////////////////////

#ifndef SIMTIMER_H
#define SIMTIMER_H

// Header files ///////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <string.h>
#include <math.h>
#include "StandardConstants.h"
#include<stdio.h>
#include<stdlib.h>

// Global Constants  //////////////////////////////////////////////////////////

enum TIMER_CTRL_CODES { ZERO_TIMER, LAP_TIMER, STOP_TIMER };

// Function Prototypes  ///////////////////////////////////////////////////////

void runTimer( int milliSeconds );
double accessTimer( int controlCode, char *timeStr );
double processTime( double startSec, double endSec, 
                           double startUSec, double endUSec, char *timeStr );
void timeToString( int secTime, int uSecTime, char *timeStr );
void *runTimerThread( void *delayVoidPtr );

void currentTimeToString( char *timeStr );

#endif // ifndef SIMTIMER_H

