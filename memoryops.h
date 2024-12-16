#ifndef MEMORYOPS_H
#define MEMORYOPS_H


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "datatypes.h"
#include "output.h"
#include "stdbool.h"




/*
Name: allocateMemoryFF
Process: First Fit allocation;
         Attempt to allocate in every segment until one works
Return: returns segment with allocated memory if successful, 
        otherwise returns NULL
*/
MemorySegmentType *allocateMemoryFF( MemorySegmentType **memoryHead, 
                                     int pid, int logicalBase, int size );

/*
Name: allocateMemoryFromSegment
Process: attempts to allocate memory from the given segment, 
           fails if there is not enough space or invalid parameter. 
         During the function, allocatedSegment will be placed before 
           the unallocated remainder of the segment(if applicable)
Return: returns segment with allocated memory if successful, 
        otherwise returns NULL, head will be updated if necessary. 
*/
MemorySegmentType *allocateMemoryFromSegment( MemorySegmentType **memoryHead, 
                                         MemorySegmentType *referenceSegment,
                                         int baseAddress, int size, int pid );

/*
Name: createMemory
Process: creates memory with capacity size. 
         data is initialized as not in use
*/
MemorySegmentType *createMemory( int capacity );

/*
Name: deallocateMemoryFromProcess
Process: deallocated memory from every segment in memory 
           dedicated to the provided pid
Return: NULL ptr
*/
MemorySegmentType *deallocateMemoryFromProcess( MemorySegmentType **memoryHead, 
                                                                       int pid );


/*
Name: deallocateMemoryFromSegment
Process: deallocates memory from referenceSegment, sets to NOT_IN_USE. 
         Combines the prev/next segments if they are also NOT_IN_USE
Return: reference to the memory segment previously containing allocated data, 
        updated value for head, if necessary
*/
MemorySegmentType *deallocateMemoryFromSegment( 
                                           MemorySegmentType **memoryHead, 
                                           MemorySegmentType *referenceSegment );

/*
Name: displayMemory
Process: displays every segment in the memory list, 
         or "No memory configured" if none
*/
void displayMemory( MemorySegmentType *memoryHead, char *label );

/*
Name: memoryAccess
Process: iterates over memory segments in the list, 
         returns a segment if the logical request is in bounds, 
           otherwise NULL
*/
MemorySegmentType *memoryAccess( MemorySegmentType *memoryHead, 
                                 int pid, int logicalBase, int size );

/*
Name: memoryOverlap
Process: returns if logical test positions overlap with any allocated 
         memory segments dedicated to the pid
*/
bool memoryOverlap( MemorySegmentType *memoryHead, 
                    int pid, int testStart, int testEnd );



#endif // MEMORYOPS_H

