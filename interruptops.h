#ifndef INTERRUPT_OPS_H
#define INTERRUPT_OPS_H

/* 
InterruptManager is a data structure that manages interrupts. 
InterruptManager can be safely managed at any time using: 
 - createInterruptManager: construct the manager - required first
 - clearInterruptManager: destruct the manager - required last
 - sendProcessToInterrupt: creates a thread that runs the process
 - getInterruptProcess: returns a process whose thread ended
 - isEmpty: returns if there are no processes waiting to be popped
 - waitingForProcesses: returns if threads are still running
*/

/*
queueLock is used to prevent race conditions allowing multiple sources
          to modify data at the same time. 
the queue is locked when: 
 - the number of processes being threaded is modified
 - nodes are being removed from the queue
 - nodes are being added to the queu
multiple sources can access data from the queue at the same time, 
   as long as it is not used to identify parts of the queue to be modified

*/

/*
at any time outside of a critical section:
 - there must be no reference to any node bewteen the queue's start and tail
    to be accessed later
 - the value of runningThreads must be greater than or equal to the number of 
    threads that could modify the queue
*/


#include "datatypes.h"
#include "pcbops.h"
#include<pthread.h>
#include<stdbool.h>

struct InterruptManager;
struct InterruptNode;


typedef struct InterruptManager
{
    struct InterruptNode *readyQueueStart;
    struct InterruptNode *readyQueueTail;
    pthread_mutex_t queueLock;
    int runningThreads;
} InterruptManager;

typedef struct InterruptNode
{
    PCBType *process;
    struct InterruptNode *next;
} InterruptNode;

typedef struct InterruptThreadArgs
{
    InterruptManager *manager;
    PCBType *process;
    int processTime;
} InterruptThreadArgs;

/*
Name: clearInterruptNode
Process: clear memory for the node
*/
InterruptNode *clearInterruptNode( InterruptNode *node );

/*
Name: clearInterruptManager
Process: waits for all threads to end, 
         clears all nodes, 
         clears memory for self
note: this will wait until all interrupts have completed 
      their time before ending
*/
InterruptManager *clearInterruptManager( InterruptManager *manager );

/*
Name: createInterruptNode
Process: allocate memory for data structure, 
         sets process as a reference to the pcbRef
*/
InterruptNode *createInterruptNode( PCBType *pcbRef );

/*
Name: create interrupt manager
Process: allocates memory to the data structure, 
         initializes as an empty queue, 
         initialize mutex, used to prevent race conditions
*/
InterruptManager *createInterruptManager();

/*
Name: getInterruptProcess
Process: pops an interrupt from the queue, if exists, 
         clears the node containing the process, 
         returns the popped process
*/
PCBType *getInterruptProcess( InterruptManager *manager );

/*
Name: isEmpty
Process: returns if there are completed processes stored in the manager
*/
bool isEmpty( InterruptManager *manager );

/*
Name: modify running threads
Process: modify the number of running threads. 
         this is the only function that modifies 
            runningThreads after initialization
         queueLock prevents runningThreads from being modified by 
            multiple calls at the same time. 
*/
void modifyRunningThreads( InterruptManager *manager, int modifier );

/*
Name: popInterrupt
Process: selects the node at the start of the queue, 
         if it exists, moves the start to the next node, 
            and returns the popped node. 
         if the queue is empty, returns NULL
         uses queueLock to prevent other sources from modifying the 
            queue at the same time
*/
InterruptNode *popInterrupt( InterruptManager *manager );

/*
Name: pushInterrupt
Process: pushes a node onto the tail
         updates tail, and start if necessary
         uses queueLock to prevent other sources from modifying 
            the queue at the same time
*/
void pushInterrupt( InterruptManager *manager, InterruptNode *toPush );

/*
Name: sendProcessToInterrupt
Process: create data structure to store thread arguments in, 
         increment the number of threads running, 
         create a thread to send the process to the interrupt manager
*/
void sendProcessToInterrupt( InterruptManager *manager, PCBType *process, 
                             int processTime );

/*
Name: sendProcessToInterruptThread
Process: store the process in an interruptNode, 
         wait for the specified time, 
         stores the node in the manager. 
         After, decrement the count of running threads
*/
void *sendProcessToInterruptThread( void *voidArgs );

/*
Name: waitingForProcesses
Process: returns if threads are being ran
*/
bool waitingForProcesses( InterruptManager *manager );

#endif // INTERRUPT_OPS_H