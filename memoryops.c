#include "memoryops.h"

/*
Name: allocateMemoryFF
Process: First Fit allocation;
         Attempt to allocate in every segment until one works
Return: returns segment with allocated memory if successful, 
        otherwise returns NULL
*/
MemorySegmentType *allocateMemoryFF( MemorySegmentType **memoryHead, 
                                     int pid, int logicalBase, int size )
{
    MemorySegmentType *wkgSeg, *allocateResult;

    // check for overlap
    if( memoryOverlap( *memoryHead, pid, logicalBase, logicalBase + size - 1 ) )
    {
        return NULL;
    }

    // iterate over all memory segments in order
    for( wkgSeg = *memoryHead; wkgSeg != NULL; wkgSeg = wkgSeg->next )
    {
        // if able to allocate memroy from the segment, return result
        allocateResult = allocateMemoryFromSegment( memoryHead, wkgSeg, 
                                                    logicalBase, size, pid );
        if( allocateResult != NULL )
        {
            return allocateResult;
        }
    }
    // done checking each unallocated segment

    // return failure
    return NULL; 
}

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
                                         int baseAddress, int size, int pid )
{
    MemorySegmentType *allocated, *unallocated;

    // return failure if already being used
    if( referenceSegment == NULL || referenceSegment->pid != NOT_IN_USE )
    {
        return NULL;
    }

    // return failure if size is too large
    if( size > referenceSegment->size )
    {
        return NULL;
    }
    // otherwise, able to extract memory from referenceSegment

    // if same size, set process attributes into the referenced segment
        // do not need to create a new node
    if( size == referenceSegment->size )
    {
        referenceSegment->pid = pid;
        referenceSegment->logicalAddress = baseAddress;
    }

    // set allocated/unallocated segments
    allocated = (MemorySegmentType *)malloc( sizeof(MemorySegmentType ) );
    unallocated = referenceSegment;

    // set position for allocated
    allocated->size = size;
    allocated->physicalAddress = unallocated->physicalAddress;

    // remove memory from beginning of unallocated
    unallocated->size -= allocated->size;
    unallocated->physicalAddress += allocated->size;

    // set process attributes for allocated
    allocated->pid = pid;
    allocated->logicalAddress = baseAddress;

    // link allocated into list (before unallocated)
    allocated->next = unallocated;
    allocated->prev = unallocated->prev;
    unallocated->prev = allocated;

    // reset head if necessary
    if( *memoryHead == referenceSegment )
    {
        *memoryHead = allocated;
    }

    // return the allocated segment
    return allocated;
}

/*
Name: createMemory
Process: creates memory with capacity size. 
         data is initialized as not in use
*/
MemorySegmentType *createMemory( int capacity )
{
    MemorySegmentType *memory = (MemorySegmentType *)malloc( sizeof(MemorySegmentType) );

    // set physical address and capacity
    memory->physicalAddress = 0;
    memory->size = capacity;

    // not in use
    memory->pid = NOT_IN_USE;

    // only item in list
    memory->next = NULL;
    memory->prev = NULL;

    return memory;
}


/*
Name: deallocateMemoryFromProcess
Process: deallocated memory from every segment in memory 
           dedicated to the provided pid
Return: NULL ptr
*/
MemorySegmentType *deallocateMemoryFromProcess( MemorySegmentType **memoryHead, 
                                                                       int pid )
{
    MemorySegmentType *wkgSegment;
    char outStr[ MAX_STR_LEN ];

    // iterate over all memory segments
    for( wkgSegment = *memoryHead; wkgSegment != NULL; wkgSegment = wkgSegment->next )
    {
        // deallocate if equal to 
        if( wkgSegment->pid == pid )
        {
            wkgSegment = deallocateMemoryFromSegment( memoryHead, wkgSegment );
        }
    }

    sprintf( outStr, "After clear process %i success\n", pid );
    displayMemory( *memoryHead, outStr );
    return NULL;
}


/*
Name: deallocateMemoryFromSegment
Process: deallocates memory from referenceSegment, sets to NOT_IN_USE. 
         Combines the prev/next segments if they are also NOT_IN_USE
Return: reference to the memory segment previously containing allocated data, 
        updated value for head, if necessary
*/
MemorySegmentType *deallocateMemoryFromSegment( 
                                           MemorySegmentType **memoryHead, 
                                           MemorySegmentType *referenceSegment )
{
    MemorySegmentType *prev, *next;

    // return failure if not in use
    if( referenceSegment == NULL || referenceSegment->pid == NOT_IN_USE )
    {
        return NULL;
    }

    prev = referenceSegment->prev;
    next = referenceSegment->next;

    // check previous node is not in use
    if( prev != NULL && prev->pid == NOT_IN_USE )
    {
        // combine with the previous node
        referenceSegment->size += prev->size;
        referenceSegment->physicalAddress = prev->physicalAddress;

        // remove the previous node from the list
        referenceSegment->prev = prev->prev;
        if( referenceSegment->prev != NULL )
        {
            referenceSegment->prev->next = referenceSegment;
        }

        free( prev );
        // reset head if removed
        if( prev == *memoryHead )
        {
            *memoryHead = referenceSegment->prev;
        }
    }

    // check next node is not in use
    if( next != NULL && next->pid == NOT_IN_USE )
    {
        // combine with the next node
        referenceSegment->size += next->size;

        // remove next node from the list
        referenceSegment->next = next->next;
        if( referenceSegment->next != NULL )
        {
            referenceSegment->next->prev = referenceSegment;
        }
        free( next );
    }

    // set to not in use
    referenceSegment->pid = NOT_IN_USE;

    return referenceSegment;
}

/*
Name: displayMemory
Process: displays every segment in the memory list, 
         or "No memory configured" if none
*/
void displayMemory( MemorySegmentType *memoryHead, char *label )
{
    char outputString[ HUGE_STR_LEN ];
    char inUseStr[ MIN_STR_LEN ];
    char pidStr[ MIN_STR_LEN ];
    int logicalStart, logicalEnd, physicalStart, physicalEnd;

    MemorySegmentType *wkgSeg;
    outputDirectly( "--------------------------------------------------\n" );
    outputDirectly( label );

    // check for no memory 
    if( memoryHead == NULL )
    {
        outputDirectly( "No memory configured\n" );
    }

    // display all values
    for( wkgSeg = memoryHead; wkgSeg != NULL; wkgSeg = wkgSeg->next )
    {
        // if the segment is not in use, set default values
        if( wkgSeg->pid == NOT_IN_USE )
        {
            sprintf( pidStr, "x" );
            sprintf( inUseStr, "Open" );
            logicalStart = 0; 
            logicalEnd = 0;
        }
        // otherwise, assume used, set values
        else
        {
            sprintf( inUseStr, "Used" );
            sprintf( pidStr, "%i", wkgSeg->pid );
            logicalStart = wkgSeg->logicalAddress;
            logicalEnd = wkgSeg->logicalAddress + wkgSeg->size - 1;
        }
        // set process independent values
        physicalStart = wkgSeg->physicalAddress;
        physicalEnd = wkgSeg->physicalAddress + wkgSeg->size - 1;

        // set into string and output without time
        sprintf( outputString, "%i [ %s, P#: %s, %i-%i ] %i\n", 
                      physicalStart, inUseStr, pidStr, 
                      logicalStart, logicalEnd, 
                      physicalEnd );
        outputDirectly( outputString );
    }
    outputDirectly( "--------------------------------------------------\n" );
}

/*
Name: memoryAccess
Process: iterates over memory segments in the list, 
         returns a segment if the logical request is in bounds, 
           otherwise NULL
*/
MemorySegmentType *memoryAccess( MemorySegmentType *memoryHead, 
                                 int pid, int logicalBase, int size )
{
    MemorySegmentType *wkgSegment;
    for( wkgSegment = memoryHead; wkgSegment != NULL; wkgSegment++ )
    {
        if( wkgSegment->pid == pid )
        {
            // check for start and end in bounds
            if( logicalBase >= wkgSegment->logicalAddress 
                && 
                logicalBase + size < wkgSegment->logicalAddress + wkgSegment->size )
            {
                return wkgSegment;;
            }
        }
    }
    return NULL;
}

/*
Name: memoryOverlap
Process: returns if logical test positions overlap with any allocated 
         memory segments dedicated to the pid
*/
bool memoryOverlap( MemorySegmentType *memoryHead, 
                    int pid, int testStart, int testEnd )
{
    MemorySegmentType *wkgSegment;
    int logicalMin, logicalMax;
    for( wkgSegment = memoryHead; wkgSegment != NULL; wkgSegment = wkgSegment->next )
    {
        if( wkgSegment->pid == pid )
        {
            logicalMin = wkgSegment->logicalAddress;
            logicalMax = wkgSegment->logicalAddress + wkgSegment->size - 1;
            // test start/end cannot be between logical min/max (case 1)
            // if not between, each are after/before min
            // if both are before or both are after, no collision
            //   only error is when start is before, end is after (case 2)
            // case 1
            if( ( testStart >= logicalMin && testStart < logicalMax )
                   || 
                ( testEnd > logicalMin && testEnd <= logicalMax ) )
            {
                return true;
            }
            // case 2
            if( testStart < logicalMin && testEnd > logicalMax ) 
            {
                return true;
            }
        }
    }
    // no collision in a segment: return failure
    return false;
}







