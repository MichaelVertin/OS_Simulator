#include "pcbops.h"


/*
Name: appendNodeToManager
Process: Adds a node toAppend to the end of manager
*/
void appendNodeToManager( PCBManagerType *manager, PCBType *toAppend )
{
    // looped list: appending is the same as prepending to current node
    manager->pcbHead = prependPCBNode( manager->pcbHead, toAppend );

}

/*
Name: clearPCBList
Process: removes all nodes from a list, starting at listNode
*/
PCBType *clearPCBList( PCBType *listNode )
{
    // continue removing nodes until none remaining
    while( listNode != NULL )
    {
        listNode = clearPCBNode( listNode );
    }

    // return NULL
    return listNode;
}

/*
Name: clearPCBNode
Process: erases simulator memory from pcbObj, unlink from list, 
         and frees struct memory
*/
PCBType *clearPCBNode( PCBType *pcbObj )
{
    PCBType *nextNode;

    if( pcbObj != NULL )
    {
        // only one remaining
        if( pcbObj->next == pcbObj )
        {
            // free, return empty pointer
            free( pcbObj );
            return NULL;
        }

        // save next node
        nextNode = pcbObj->next;

        // configData was not constructed in PCBType: do not destruct

        // link prev and next
        pcbObj->prev->next = pcbObj->next;
        pcbObj->next->prev = pcbObj->prev;

        // free pcbObj
        free( pcbObj );

        // return saved next node
        return nextNode;
    }

    // return NULL (no new node to access)
    return NULL;
}

/*
Name: createPCBList
Process: returns NULL, representing an empty list
*/
PCBType *createPCBList()
{
    return NULL;
}

/*
Name: createPCBManager
Process: creates a PCBManagerType containing all of the 
         data in the opCodePtr list
*/
PCBManagerType *createPCBManager( OpCodeType *opCodePtr, 
                                  ConfigDataType *configPtr )
{
    PCBManagerType *manager = (PCBManagerType *)malloc(sizeof(PCBManagerType));

    // create a opCodePtr to iterate over the list
    OpCodeType *wkgOpCodePtr = opCodePtr;
    PCBType *newNode;

    // skip start code
    wkgOpCodePtr = wkgOpCodePtr->nextNode;

    // create PCB list
    manager->pcbHead = createPCBList();

    // iterate until sys end
        // not ( sys and end )
    while( !( compareString( wkgOpCodePtr->command, "sys" ) == 0 && 
              compareString( wkgOpCodePtr->strArg1, "end" ) == 0 ) )
    {
        // construct an pcb node
        newNode = createPCBNode( &wkgOpCodePtr, configPtr );

        // set to the next unused pid, update
        newNode->pid = manager->unusedPid;
        manager->unusedPid++;

        // add newNode into the pcb list
        appendNodeToManager( manager, newNode );
    }

    // skip sys end
    wkgOpCodePtr = wkgOpCodePtr->nextNode;

    // initialize memory
    manager->memory = createMemory( configPtr->memAvailable );

    return manager;
}

/*
Name: createPCBNode
Process: creates a PCBNode from the data in wkgOpCodePtr
Parameters/Input: wkgOpCodePtr must be pointed at "app start"
Parameters/Output: wkgOpCodePtr will be pointed at the node
                   following "app end"
*/
PCBType *createPCBNode( OpCodeType **wkgOpCodePtr, ConfigDataType *configPtr )
{
    // initialize variables
    PCBType *pcbResult = (PCBType *)malloc( sizeof( PCBType ) );

    // assume start at "app start"
    // advance past app start
    *wkgOpCodePtr = (*wkgOpCodePtr)->nextNode;

    // set result's head to the current element
    pcbResult->programHead = *wkgOpCodePtr;

    // iterate to end of process (app end)
    while( compareString( (*wkgOpCodePtr)->command, "app" ) != 0 && 
           compareString( (*wkgOpCodePtr)->strArg1, "end" ) != 0 )
    {
        // move to the next node
        *wkgOpCodePtr = (*wkgOpCodePtr)->nextNode;
    }

    // complete by skipping "app end", and storing exclusive end ptr
    pcbResult->programEnd = *wkgOpCodePtr;
    *wkgOpCodePtr = (*wkgOpCodePtr)->nextNode;

    // set default values
    pcbResult->programCounter = pcbResult->programHead;
    pcbResult->completedProgramCycles = 0;
    pcbResult->consecutiveCycles = 0;
    pcbResult->remainingTotalTime = getPCBTime( pcbResult, configPtr );
    pcbResult->next = pcbResult;
    pcbResult->prev = pcbResult;
    copyString( pcbResult->state, "NEW" );

    return pcbResult;
}

/*
Name: getCycleRate
Process: calculates the time per cycle for a program
*/
int getCycleRate( OpCodeType *opCode, ConfigDataType *configPtr )
{
    // "dev" uses i/o cycle
    if( compareString( opCode->command, "dev" ) == 0 )
    {
        return configPtr->ioCycleRate;
    }
    else if( compareString( opCode->command, "mem" ) == 0 )
    {
        return 0; // no delay for memory operation
    }

    // otherwise, assume proc cycle
    return configPtr->procCycleRate;
}

/*
Name: getPCBTime
Process: get's total time PCB requires to finish
*/
int getPCBTime( PCBType *pcb, ConfigDataType *configPtr )
{
    int sum = 0;
    OpCodeType *wkgProgram;
    for( wkgProgram = pcb->programHead; 
             wkgProgram != pcb->programEnd;
                   wkgProgram = wkgProgram->nextNode )
    {
        sum += getCycleRate( wkgProgram, configPtr ) * wkgProgram->intArg2;
    }

    return sum;
}

/*
Name: prependPCBNode
Process: places newNode in front of the reference node
*/
PCBType *prependPCBNode( PCBType *refListNode, PCBType *newNode )
{
    // return new node if list is empty
    if( refListNode == NULL )
    {
        // link newNode into itself (looped list)
        newNode->prev = newNode;
        newNode->next = newNode;
        return newNode;
    }

    // otherwise, pcbList is not empty
    // link new into list
    newNode->prev = refListNode->prev;
    newNode->next = refListNode;

    // link list into new
    refListNode->prev->next = newNode;
    refListNode->prev = newNode;

    return refListNode;
}

/*
Name: setPCBState
Process: set's pcbObj's state to newState, 
         displays the change in state
*/
void setPCBState( PCBType *pcbObj, char *newState )
{
    char outputString[ HUGE_STR_LEN ];
    if( pcbObj != NULL )
    {
        sprintf( outputString, "OS: Process %i set from %s to %s\n", 
                                        pcbObj->pid, pcbObj->state, newState );
        copyString( pcbObj->state, newState );
        output( outputString );
    }
}

/*
Name: taskDataToString
Process: converts the pcb's data into a string
Parameters/Output: requires taskStr has enough memory allocated
                   adds start/end into string, depending on 'start'
*/
bool taskDataToString( char *taskStr, PCBType *pcb, bool start )
{
    OpCodeType *opCode = pcb->programCounter;

    // case dev command
    if( compareString( opCode->command, "dev" ) == 0 )
    {
        sprintf( taskStr, "Process: %i, %s %sput operation", 
                          pcb->pid, opCode->strArg1, opCode->inOutArg );
    }
    // case cpu process
    else if( compareString( opCode->command, "cpu" ) == 0 )
    {
        sprintf( taskStr, "Process: %i, %s %s operation", 
                          pcb->pid, opCode->command, opCode->strArg1 );
    }
    // case mem process
    else if( compareString( opCode->command, "mem" ) == 0 )
    {
        sprintf( taskStr, "Process: %i, %s %s request (%i,%i)", 
                          pcb->pid, opCode->command, opCode->strArg1, 
                          opCode->intArg2, opCode->intArg3 );
    }
    // otherwise, none of the above: not a task
    else
    {
        taskStr[0] = NULL_CHAR;
        return false;
    }

    // declare if starting or ending
    if( start )
    {
        concatenateString( taskStr, " start\n"  );
    }
    else
    {
        concatenateString( taskStr, " end\n"  );
    }

    // return success
    return true;
}





