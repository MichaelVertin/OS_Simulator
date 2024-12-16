#ifndef PCB_H
#define PCB_H

#include "datatypes.h"
#include "StringUtils.h"
#include "memoryops.h"
#include "output.h"





/*
Name: appendNodeToManager
Process: Adds a node toAppend to the end of manager
*/
void appendNodeToManager( PCBManagerType *manager, PCBType *toAppend );

/*
Name: clearPCBList
Process: removes all nodes from a list, starting at listNode
*/
PCBType *clearPCBList( PCBType *listNode );

/*
Name: clearPCBNode
Process: erases simulator memory from pcbObj, unlink from list, 
         and frees struct memory
*/
PCBType *clearPCBNode( PCBType *pcbObj );


/*
Name: createPCBList
Process: returns NULL, representing an empty list
*/
PCBType *createPCBList();

/*
Name: createPCBManager
Process: creates a PCBManagerType containing all of the 
         data in the opCodePtr list
*/
PCBManagerType *createPCBManager( OpCodeType *opCodePtr, 
                                  ConfigDataType *configPtr );

/*
Name: createPCBNode
Process: creates a PCBNode from the data in wkgOpCodePtr
Parameters/Input: wkgOpCodePtr must be pointed at "app start"
Parameters/Output: wkgOpCodePtr will be pointed at the node
                   following "app end"
*/
PCBType *createPCBNode( OpCodeType **wkgOpCodePtr, ConfigDataType *configPtr );

/*
Name: getCycleRate
Process: calculates the time per cycle for a program
*/
int getCycleRate( OpCodeType *opCode, ConfigDataType *configPtr );

/*
Name: getPCBTime
Process: get's total time PCB requires to finish
*/
int getPCBTime( PCBType *pcb, ConfigDataType *configPtr );

/*
Name: prependPCBNode
Process: places newNode in front of the reference node
*/
PCBType *prependPCBNode( PCBType *refListNode, PCBType *newNode );

/*
Name: setPCBState
Process: set's pcbObj's state to newState, 
         displays the change in state
*/
void setPCBState( PCBType *pcbObj, char *newState );

/*
Name: taskDataToString
Process: converts the pcb's data into a string
Parameters/Output: requires taskStr has enough memory allocated
                   adds start/end into string, depending on 'start'
*/
bool taskDataToString( char *taskStr, PCBType *pcb, bool start );


#endif // PCB_H

