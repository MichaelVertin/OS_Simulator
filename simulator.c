/* TODOS: 
Memory access fail needs to cause seg fault
When interrupt found: create function

*/

/* ERRORS:
Memory not cleared

*/

#include "simulator.h"
const bool START_OPERATION = true;
const bool DO_NOT_START_OPERATION = false;

// simulator function
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
{
    PCBManagerType *pcbManager;
    InterruptManager *interruptManager;
    PCBType *interruptPCB;
    char outputString[ HUGE_STR_LEN ];
    bool selectNextProgramFlag, selectNextProcessFlag;
    bool idling = false;
    int operationTime;
    PCBType *pcb; // temporary variable used to reduce code
    OpCodeType *opCode; // temporary variable used to reduce code

    // initialize interrupt manager
    interruptManager = createInterruptManager();

    // identify if code is being logged to file/monitor
    setFileOutput( configPtr->logToCode == LOGTO_FILE_CODE || 
                   configPtr->logToCode == LOGTO_BOTH_CODE );
    setConsoleOutput( configPtr->logToCode == LOGTO_MONITOR_CODE || 
                      configPtr->logToCode == LOGTO_MONITOR_CODE );

    // create pcb manager from config data
    pcbManager = createPCBManager( metaDataMstrPtr, configPtr );

    // display title
    outputDirectly( "Simulator Run\n" );
    outputDirectly( "-------------\n\n" );

    // prime timer
    accessTimer( ZERO_TIMER, NULL );

    // start simulator
    output( "OS: Simulator start\n" );

    // set all processes to ready
    setAllProcessStates( pcbManager, "READY" );

    // display memory
    displayMemory( pcbManager->memory, "After memory initialization\n" );

    // select the first cycle
    pcbManager->currentPcb = pcbManager->pcbHead;
    setPCBToRunning( pcbManager->currentPcb, START_OPERATION );

    // iterate while a process is active or waiting for processes
    while( !allProcessesExited( pcbManager ) || 
           waitingForProcesses( interruptManager )
         )
    {
        // default, assume nothing needs to be modified
        selectNextProcessFlag = false;
        selectNextProgramFlag = false;

        // check for an io operation is ready to interrupt
        interruptPCB = getInterruptProcess( interruptManager );
        if( interruptPCB != NULL )
        {
            // cpu is idling (no processes will be interrupted)
            // select interruptPCB
            if( idling )
            {
                // interrupt will be selected, no longer idle
                output( "OS: CPU interrupt, end idle\n" );
                idling = false;
                outputNewline();

                sprintf( outputString, "OS: Interrupted by process %i\n", 
                                       interruptPCB->pid );
                output( outputString );

                // start running the interrupting process
                setPCBState( interruptPCB, "READY" );
                setPCBToRunning( interruptPCB, DO_NOT_START_OPERATION );
                pcbManager->currentPcb = interruptPCB;
                selectNextProgramFlag = true;
            }
            // cpu is idling (a process will be interrupted)
            // do not select interruptPCB
            else
            {
                /*
                Note: In this program, processes will not display that an 
                      operation began/ended unless running, and two process
                      will not be running at the same time. 
                      Therefore, the interrupted process will be blocked and 
                      the interrupting process will be running until the 
                      interrupt ends.
                */
                // block the current process
                output( "OS: Blocking current process for interrupt\n" );
                setPCBState( pcbManager->currentPcb, "BLOCKED" );
                outputNewline();

                // start running the interrupting process
                sprintf( outputString, "OS: Interrupted by process %i\n", 
                                       interruptPCB->pid );
                output( outputString );
                setPCBState( interruptPCB, "RUNNING" );

                // terminate the IO operation by selecting the next operation
                selectNextProcessFlag = simSelectNextOperation( interruptPCB );

                // check for process ended
                if( selectNextProgramFlag )
                {
                    // interruptPCB was never selected, so don't interfere with 
                    //   the selection process. 
                    //   Instead, just end the interrupting state
                    setPCBState( interruptPCB, "EXIT" );
                    selectNextProcessFlag = false;
                }
                // otherwise, not done: the interrupting process is now ready
                else
                {
                    setPCBState( interruptPCB, "READY" );
                }

                outputNewline();

                // continue running the current process
                output( "OS: Done interrupting, continue with current\n" );
                setPCBState( pcbManager->currentPcb, "RUNNING" );
            }
        }

        // check pcbManager has processes
        else if( pcbManager->currentPcb != NULL )
        {
            // store current pcb/opCode for quick access
            pcb = pcbManager->currentPcb;
            opCode = pcb->programCounter;

            // check for memory operation
            if( compareString( opCode->command, "mem" ) == 0 )
            {
                // perform memory operation
                performMemoryOperation( pcbManager, opCode );
                selectNextProgramFlag = true;
            }

            // otherwise, check for preemptive input/output operation
            else if( isPreemptive( configPtr ) && 
                     compareString( opCode->command, "dev" ) == 0 )
            {
                // get the process time, reduce the remaining time here
                operationTime = getCycleRate( opCode, configPtr ) * opCode->intArg2;
                pcb->remainingTotalTime -= operationTime;

                // send the operation to the interrupt manager
                setPCBToBlocking( pcb );
                sendProcessToInterrupt( interruptManager, pcb, operationTime );
                selectNextProcessFlag = true;
            }

            // otherwise, assume uses blocking timer 
            //     (cpu operation or io non-preemptive)
            else
            {
                // display start of task if 0 cycles completed
                if( pcb->completedProgramCycles == 0 )
                {
                    taskDataToString( outputString, pcb, PROCESS_NOT_ENDED );
                    output( outputString );
                }

                // reset quantum cycle count before looping
                pcb->consecutiveCycles = 0;

                // loop for cpu operation
                // iterate until cpu process needs to end
                while( 
                       // cycles not completed
                       ( pcb->completedProgramCycles < opCode->intArg2 ) 
                       && 
                       // nothing in interrupt manager
                       isEmpty( interruptManager )
                       && 
                       // quantum cycle limit not met
                       ( !reachedQuantumCycleLimit( pcb, configPtr ) )
                     )
                {
                    runCycle( pcb, configPtr );
                }

                // select the next program if all cycles completed
                selectNextProgramFlag = pcb->completedProgramCycles >= 
                                        opCode->intArg2;

                // if not completed, and met the quantum cycle limit
                //    timing out is redundant if the operation already ended
                if( !selectNextProgramFlag && 
                    reachedQuantumCycleLimit( pcb, configPtr ) )
                {
                    sprintf( outputString, "OS: Process %i quantum time out\n",
                                                                     pcb->pid );
                    output( outputString );

                    // skip to the next process
                    selectNextProcessFlag = true;
                }
            }
        }

        // first move to next operation if requested
        if( selectNextProgramFlag )
        {
            // check end of process
            if( simSelectNextOperation( pcbManager->currentPcb ) )
            {
                sprintf( outputString, "OS: Process %i ended\n", 
                                               pcbManager->currentPcb->pid );
                output( outputString );
                // clear memory from the current process
                deallocateMemoryFromProcess( &pcbManager->memory, 
                                             pcbManager->currentPcb->pid );

                // clear the current process
                setPCBState( pcbManager->currentPcb, "EXIT" );

                // select the next process
                selectNextProcessFlag = true;
            }
        }

        // next, move to next process if necessary or requested
        if( selectNextProcessFlag )
        {
            // select the process for the simulator
            simSelectNextProcess( pcbManager, configPtr->cpuSchedCode );
        }

        // check for no processes running in pcbManager
        if( pcbManager->currentPcb == NULL )
        {
            // if not already idling, begin idling
            if( !idling )
            {
                sprintf( outputString, "OS: CPU idle, all active processes blocked\n" );
                output( outputString );
                idling = true;
            }
        }
    }

    // clear data structures
    clearPCBList( pcbManager->pcbHead );

    // display memory after clearing memory
    displayMemory( pcbManager->memory, "After clear all process success\n" );

    // display simulation end
    output( "OS: Simulation End\n" );

    // output results to file
    displayDataToFile( configPtr->logToFileName );
}

/*
Name: simSelectNextOperation
Process: moves to the next operation node, 
         returns if the process ended
*/
bool simSelectNextOperation( PCBType *pcb )
{
    char outputString[ HUGE_STR_LEN ];
    taskDataToString( outputString, pcb, PROCESS_ENDED );
    output( outputString );

    // move to the next program, reset state
    pcb->programCounter = pcb->programCounter->nextNode;
    pcb->completedProgramCycles = 0; 

    // return if process ended
    return pcb->programCounter == pcb->programEnd;
}


/*
Name: simSelectNextProcess
Process: selects the next process by schedCode, 
         starts running the next process
*/
void simSelectNextProcess( PCBManagerType *manager, ConfigDataCodes schedCode )
{
    PCBType *nextProcess;

    // reset consecutive process cycles
    manager->currentPcb->consecutiveCycles = 0;

    // identify/select the next process
    nextProcess = getNextProcessByCode( manager, schedCode );

    // only select the process if not already selected
    if( nextProcess != manager->currentPcb )
    {
        manager->currentPcb = nextProcess;
        outputNewline();
        setPCBToRunning( manager->currentPcb, START_OPERATION );
    }

}

/*
Name: getNextProcessByCode
Process: selects the next process based on the current process and processCode
         if processCode is PROCESS_NOT_ENDED, the process is only 
            modified when the processCode allows processes to 
            stop running before complete
*/
PCBType *getNextProcessByCode( PCBManagerType *manager, 
                               ConfigDataCodes schedCode )
{
    PCBType *nextProcess = NULL, *shortestProcess;
    PCBType *iterator = NULL;

    // first come first serve: choose next process from head (inclusive)
    if( schedCode == CPU_SCHED_FCFS_N_CODE || 
        schedCode == CPU_SCHED_FCFS_P_CODE )
    {
        nextProcess = getNextReadyProcess( manager->pcbHead, &iterator );
    }

    // srtf-p behaves the same as sjf-n
    else if( schedCode == CPU_SCHED_SRTF_P_CODE || 
             schedCode == CPU_SCHED_SJF_N_CODE )
    {
        // start iterating from head
        nextProcess = getNextReadyProcess( manager->pcbHead, &iterator );

        // start with first process as shortest
        shortestProcess = nextProcess;

        // identify smallest processes by looping
        while( nextProcess != NULL )
        {
            // set new shortest if applicable
            if( nextProcess->remainingTotalTime < 
                shortestProcess->remainingTotalTime )
            {
                shortestProcess = nextProcess;
            }
            nextProcess = getNextReadyProcess( manager->pcbHead, &iterator );
        }

        // set next process to the shortest one
        nextProcess = shortestProcess;
    }
    // RR-P: choose next process from current (exlusive
    else if( schedCode == CPU_SCHED_RR_P_CODE )
    {
        // iterate to the next ready process
        nextProcess = getNextReadyProcess( manager->currentPcb->next, &iterator );
    }
    // otherwise, default to FCFS
    else
    {
        nextProcess = getNextProcessByCode( manager, CPU_SCHED_FCFS_N_CODE );
    }
    return nextProcess;
}




/*
Name: performMemoryOperation
Process: performs a memory operation as described by the programCounter
         if programCounter is not a mem command, does nothing
*/
void performMemoryOperation( PCBManagerType *pcbManager, OpCodeType *programCounter )
{
    char memoryLabel[ HUGE_STR_LEN ];

    // verify memory operation
    if( compareString( programCounter->command, "mem" ) == 0 )
    {
        // check for memory allocation
        if( compareString( programCounter->strArg1, "allocate" ) == 0 )
        {
            // check for collision
            if( memoryOverlap( pcbManager->memory, pcbManager->currentPcb->pid, 
                               programCounter->intArg2, programCounter->intArg3 ) )
            {
                copyString( memoryLabel, "After allocate failure\n" );
            }

            // allocate memory
            else if( allocateMemoryFF( &pcbManager->memory, 
                         pcbManager->currentPcb->pid, programCounter->intArg2, 
                                                        programCounter->intArg3 ) )
            {
                copyString( memoryLabel, "After allocate success\n" );
            }

            // otherwise, no space for memory
            else
            {
                copyString( memoryLabel, "not enough memory\n" );
            }
        }

        // otherwise, assume attempt to access memory
        else
        {
            // attempt to access memory
            if( memoryAccess( pcbManager->memory, pcbManager->currentPcb->pid, 
                              programCounter->intArg2, programCounter->intArg3 ) 
                 != NULL )
            {
                copyString( memoryLabel, "After access success\n" );
            }
            else
            {
                copyString( memoryLabel, "After access failure\n" );
            }
        }

        // display memory
        displayMemory( pcbManager->memory, memoryLabel );
    }
}

/*
Name: runCycle
Process: blocks program for one cycle, 
         adjusts timing variables as needed
*/
void runCycle( PCBType *pcb, ConfigDataType *configPtr )
{
    int cycleTime = getCycleRate( pcb->programCounter, configPtr );
    runTimer( cycleTime );
    pcb->remainingTotalTime -= cycleTime;
    pcb->completedProgramCycles += 1;
    pcb->consecutiveCycles += 1;
}

/*
Name: setPCBToBlocking
Process: sets pcb to blocked state, 
         outputs blocking message
*/
void setPCBToBlocking( PCBType *pcb )
{
    char outputString[ HUGE_STR_LEN ];
    if( pcb != NULL )
    {
        sprintf( outputString, "OS: Process %i blocked for %sput operation\n", 
                 pcb->pid, pcb->programCounter->inOutArg );
        output( outputString );
        setPCBState( pcb, "BLOCKED" );
    }
}

/*
Name: set PCBToRunning
Process: sets pcb to running state, 
         outputs selected message
         if startOperation is set, displays that the process started
*/
void setPCBToRunning( PCBType *pcb, bool startOperation )
{
    char outputString[ HUGE_STR_LEN ];
    if( pcb != NULL )
    {
        sprintf( outputString, "OS: Process %i selected with %i ms remaining\n", 
                 pcb->pid, pcb->remainingTotalTime );
        output( outputString );
        setPCBState( pcb, "RUNNING" );
        if( startOperation )
        {
            taskDataToString( outputString, pcb, PROCESS_NOT_ENDED );
            output( outputString );
        }
    }
}

/*
Name: allProcessesExited
Process: returns if every process is in an EXIT state
*/
bool allProcessesExited( PCBManagerType *manager )
{
    PCBType *wkgPCB = NULL, *iter = NULL;
    wkgPCB = iterateNextProcess( manager->pcbHead, &iter );
    while( wkgPCB != NULL )
    {
        if( compareString( wkgPCB->state, "EXIT" ) != 0 )
        {
            return false;
        }
        wkgPCB = iterateNextProcess( manager->pcbHead, &iter );
    }
    return true;
}

// name: iterateNextProcess
// process: 
// iterates over looped PCBType list
// at the beginning/end of any call:
//   --- original iter's value has not been returned
//   --- return original iter's value
// at end of recursion
//   --- original iter's value is start (already been returned)
PCBType *iterateNextProcess( PCBType *start, PCBType **iter )
{
    // check at start
    if( *iter == start )
    {
        // end iterator
        return NULL;
    }

    // set to start on first call
    if( *iter == NULL )
    {
        *iter = start;
    }

    // advance the iterator
    *iter = (*iter)->next;

    // return the value before advanced
    return (*iter)->prev;
}

/*
Name: getNextReadyProcess
Process: iterates to the next ready process, 
         returns NULL if none found
*/
PCBType *getNextReadyProcess( PCBType *start, PCBType **iter )
{
    PCBType *nextProcess;

    // get the next process
    nextProcess = iterateNextProcess( start, iter );

    // repeat if nextProcess is in a non-ready non-running state
    if( nextProcess != NULL && 
         ( !compareString( nextProcess->state, "READY" ) == 0 && 
           !compareString( nextProcess->state, "RUNNING" ) == 0 )
      )
    {
        nextProcess = getNextReadyProcess( start, iter );
    }

    return nextProcess;
}

/*
Name: setAllProcessStates
Process: sets state of all processes to specified state
*/
void setAllProcessStates( PCBManagerType *manager, char *state )
{
    PCBType *iterator = NULL;
    PCBType *nextProcess;

    nextProcess = iterateNextProcess( manager->pcbHead, &iterator );
    while( nextProcess != NULL )
    {
        setPCBState( nextProcess, state );
        nextProcess = iterateNextProcess( manager->pcbHead, &iterator );
    }
}

/*
Name: reachedQuantumCycleLimit
Process: returns if quantum cycle is being used and 
         the process has ran for the cycle limit consecutively
*/
bool reachedQuantumCycleLimit( PCBType *pcb, ConfigDataType *configPtr )
{
    // skip if not round robin
    if( configPtr->cpuSchedCode != CPU_SCHED_RR_P_CODE )
    {
        return false;
    }

    // return if consecutive cycles met quantum cycle limit
    return pcb->consecutiveCycles >= configPtr->quantumCycles;
}

/*
Name: isPreemptive
Process: returns if the configPtr uses a preemptive code
*/
bool isPreemptive( ConfigDataType *configPtr )
{
    return configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE || 
           configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE || 
           configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE;
}







