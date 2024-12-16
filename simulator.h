#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "simtimer.h"
#include "output.h"
#include "pcbops.h"
#include "interruptops.h"


typedef enum 
{
    PROCESS_ENDED, 
    PROCESS_NOT_ENDED
} SELECT_PROCESS_CODE;

// simulator function
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

/*
Name: getNextProcessByCode
Process: selects the next process based on the current process and processCode
         if processCode is PROCESS_NOT_ENDED, the process is only 
            modified when the processCode allows processes to 
            stop running before complete
*/
PCBType *getNextProcessByCode( PCBManagerType *manager, 
                               ConfigDataCodes dataCode );

/*
Name: performMemoryOperation
Process: performs a memory operation as described by the programCounter
         if programCounter is not a mem command, does nothing
*/
void performMemoryOperation( PCBManagerType *pcbManager, OpCodeType *programCounter );

/*
Name: selectNextCycle
Process: if the simulator has not began, sets the first process. 
         if a process ended, selects the next process. 
         if a program ended, selects the next program. 
         Because memory operations do not use cycle time, 
           any memory operations are executed, 
           before returning to the caller
*/
void selectNextCycle( PCBManagerType *pcbManager, ConfigDataType *configPtr );

void runCycle( PCBType *pcb, ConfigDataType *configPtr );
void setPCBToBlocking( PCBType *pcb );
void setPCBToRunning( PCBType *pcb, bool startOperation );
bool allProcessesExited( PCBManagerType *manager );
PCBType *getNextReadyProcessIterate( char *cmd, PCBType *start );
PCBType *getProcessIterate( PCBType *start, PCBType **iter, bool *iterating );
PCBType *getNextReadyProcess( PCBType *start, PCBType **iter );
PCBType *iterateNextProcess( PCBType *start, PCBType **iter );
void setAllProcessStates( PCBManagerType *manager, char *state );
bool reachedQuantumCycleLimit( PCBType *pcb, ConfigDataType *configPtr );
void simSelectNextProcess( PCBManagerType *manager, ConfigDataCodes schedCode );
bool simSelectNextOperation( PCBType *pcb );
bool isPreemptive( ConfigDataType *configPtr );

#endif // SIMULATOR_H
