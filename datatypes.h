/*
Provided by Dr. Michael Leverington NAU CS480 Fall2023
*/

// protect from multiple compiling
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"
#include <pthread.h>

// GLOBAL CONSTANTS - may be used in other files
#define NOT_IN_USE -1

typedef enum { CMB_STR_LEN = 5, 
               IO_ARG_STR_LEN = 5, 
               STR_ARG_LEN = 15 
             } OpCodeArrayCapacity;

// Five state data structure for processes
typedef enum { NEW_STATE, 
               READY_STATE, 
               RUNNING_STATE, 
               BLOCKED_STATE, 
               EXIT_STATE
             } ProcessState;

typedef enum { CFG_FILE_ACCESS_ERR, 
               CFG_CORRUPT_DESCRIPTOR_ERR, 
               CFG_DATA_OUT_OF_RANGE_ERR, 
               CFG_CORRUPT_PROMPT_ERR, 
               CFG_VERSION_CODE, 
               CFG_MD_FILE_NAME_CODE, 
               CFG_CPU_SCHED_CODE, 
               CFG_QUANT_CYCLES_CODE, 
               CFG_MEM_DISPLAY_CODE, 
               CFG_MEM_AVAILABLE_CODE, 
               CFG_PROC_CYCLES_CODE, 
               CFG_IO_CYCLES_CODE, 
               CFG_LOG_TO_CODE, 
               CFG_LOG_FILE_NAME_CODE
             } ConfigCodeMessages;

typedef enum { CPU_SCHED_SJF_N_CODE, 
               CPU_SCHED_SRTF_P_CODE, 
               CPU_SCHED_FCFS_P_CODE, 
               CPU_SCHED_RR_P_CODE, 
               CPU_SCHED_FCFS_N_CODE, 
               LOGTO_MONITOR_CODE, 
               LOGTO_FILE_CODE, 
               LOGTO_BOTH_CODE, 
               NON_PREEMPTIVE_CODE, 
               PREEMPTIVE_CODE 
             } ConfigDataCodes;

typedef struct ConfigDataTypeStruct
{
    double version;
    char metaDataFileName[ STD_STR_LEN ];
    int cpuSchedCode; // see configCodeToString
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode; // see configCodeToString
    char logToFileName[ MAX_STR_LEN ];
} ConfigDataType;

typedef struct OpCodeTypeStruct
{
    int pid;
    int intArg2, intArg3;
    struct OpCodeTypeStruct *nextNode;
    double opEndTime;
    char command[ MAX_STR_LEN ];
    char inOutArg[ MAX_STR_LEN ];
    char strArg1[ MAX_STR_LEN ];
} OpCodeType;






typedef struct PCBStateStruct
{
    OpCodeType *opCodeCurrent;
    char stateStr[ MIN_STR_LEN ];
    int elapsedTime;
    int requiredTime;
    int elapsedTaskCycles;
} PCBStateType;



struct MemorySegmentType;
struct PCBManagerType;
struct ProcessTableType;
struct PCBType;


// table to hold processes control blocks
typedef struct ProcessTableStruct
{
    int unusedPid;
    struct PCBType *pCBCurrent;

} ProcessTableType;

typedef struct MemorySegmentType
{
    // previous and next nodes
    struct MemorySegmentType *next, *prev;

    // physical/logical address
    int physicalAddress, logicalAddress;
    int size;

    // reference to process
    int pid;
} MemorySegmentType;

typedef struct PCBType
{
    int pid;
    OpCodeType *programHead;
    OpCodeType *programCounter, *programEnd;

    int completedProgramCycles;
    int consecutiveCycles;
    int remainingTotalTime;
    char state[ STD_STR_LEN ];

    struct PCBType *prev, *next;
} PCBType;

typedef struct PCBManager
{
    PCBType *pcbHead;
    ConfigDataType *configPtr;
    int unusedPid;
    struct MemorySegmentType *memory;

    PCBType *currentPcb;
} PCBManagerType;





#endif // DATATYPES_H

