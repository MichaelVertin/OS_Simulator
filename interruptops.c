#include "interruptops.h"

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


/*
Name: clearInterruptNode
Process: clear memory for the node
*/
InterruptNode *clearInterruptNode( InterruptNode *node )
{
    free( node );
    return NULL;
}

/*
Name: clearInterruptManager
Process: waits for all threads to end, 
         clears all nodes, 
         clears memory for self
note: this will wait until all interrupts have completed 
      their time before ending
*/
InterruptManager *clearInterruptManager( InterruptManager *manager )
{
    PCBType *interruptProcess;

    while( !isEmpty( manager ) || waitingForProcesses( manager ) )
    {
        interruptProcess = getInterruptProcess( manager );
        clearPCBNode( interruptProcess );
    }

    free( manager );
    return NULL;
}

/*
Name: createInterruptNode
Process: allocate memory for data structure, 
         sets process as a reference to the pcbRef
*/
InterruptNode *createInterruptNode( PCBType *pcbRef )
{
    InterruptNode *newNode = (InterruptNode *)malloc( sizeof( InterruptNode ) );

    newNode->process = pcbRef;
    newNode->next = NULL;

    return newNode;
}

/*
Name: create interrupt manager
Process: allocates memory to the data structure, 
         initializes as an empty queue, 
         initialize mutex, used to prevent race conditions
*/
InterruptManager *createInterruptManager()
{
    InterruptManager *manager = (InterruptManager *)malloc( 
                                                   sizeof( InterruptManager ) );
    manager->readyQueueStart = NULL;
    manager->readyQueueTail = NULL;
    pthread_mutex_init( &manager->queueLock, NULL );
    manager->runningThreads = 0;
    
    return manager;
}

/*
Name: getInterruptProcess
Process: pops an interrupt from the queue, if exists, 
         clears the node containing the process, 
         returns the popped process
*/
PCBType *getInterruptProcess( InterruptManager *manager )
{
    PCBType *poppedProcess;
    InterruptNode *poppedNode = popInterrupt( manager );
    if( poppedNode == NULL )
    {
        return NULL;
    }
    poppedProcess = poppedNode->process;
    clearInterruptNode( poppedNode );
    return poppedProcess;
}

/*
Name: isEmpty
Process: returns if there are completed processes stored in the manager
*/
bool isEmpty( InterruptManager *manager )
{
    // if start is NULL, list is empty
    return manager->readyQueueStart == NULL;
}

/*
Name: modify running threads
Process: modify the number of running threads. 
         this is the only function that modifies 
            runningThreads after initialization
         queueLock prevents runningThreads from being modified by 
            multiple calls at the same time. 
*/
void modifyRunningThreads( InterruptManager *manager, int modifier )
{
    // when modifying runningThreads, 
    //    lock other sources out of the critical section
    pthread_mutex_lock( &manager->queueLock );

    // modify runningThreads when in critical section
    manager->runningThreads += modifier;

    // modification complete, allow other sources to modify runningThreads
    pthread_mutex_unlock( &manager->queueLock );
}

/*
Name: popInterrupt
Process: selects the node at the start of the queue, 
         if it exists, moves the start to the next node, 
            and returns the popped node. 
         if the queue is empty, returns NULL
         uses queueLock to prevent other sources from modifying the 
            queue at the same time
*/
InterruptNode *popInterrupt( InterruptManager *manager )
{
    InterruptNode *poppedNode;

    // prevent other sources from modifying the queue 
    //    while changes are being made
    pthread_mutex_lock( &manager->queueLock );

    // select the first node to be removed
    poppedNode = manager->readyQueueStart;

    // if the node doesn't exist, return access to the queue and exit
    if( poppedNode == NULL )
    {
        pthread_mutex_unlock( &manager->queueLock );
        return NULL;
    }
    // otherwise, the node does exist

    // remove the node from the queue by setting start to the next item
    manager->readyQueueStart = manager->readyQueueStart->next;
    if( manager->readyQueueStart == NULL )
    {
        // update tail if the last node was removed
        manager->readyQueueTail = NULL;
    }

    // done modifying queueLock, return access to other sources
    pthread_mutex_unlock( &manager->queueLock );
    
    // poppedNode is no longer accessible from queue: 
    //   safe to reference outside of critical section
    return poppedNode;
}

/*
Name: pushInterrupt
Process: pushes a node onto the tail
         updates tail, and start if necessary
         uses queueLock to prevent other sources from modifying 
            the queue at the same time
*/
void pushInterrupt( InterruptManager *manager, InterruptNode *toPush )
{
    // prevent other sources from modifying the queue
    pthread_mutex_lock( &manager->queueLock );

    // if empty, start and tail have the same value
    if( manager->readyQueueTail == NULL )
    {
        manager->readyQueueStart = toPush;
        manager->readyQueueTail = toPush;
    }
    // otherwise, not empty: store the value after the tail, 
    //   update the tail to include the new node
    else
    {
        manager->readyQueueTail->next = toPush;
        manager->readyQueueTail = toPush;
    }

    // changes complete, allow other sources to modify the queue
    pthread_mutex_unlock( &manager->queueLock );
}

/*
Name: sendProcessToInterrupt
Process: create data structure to store thread arguments in, 
         increment the number of threads running, 
         create a thread to send the process to the interrupt manager
*/
void sendProcessToInterrupt( InterruptManager *manager, PCBType *process, 
                             int processTime )
{
    pthread_t tempId;
    InterruptThreadArgs *threadArgs = (InterruptThreadArgs *)malloc( 
                                                sizeof( InterruptThreadArgs ) );
    threadArgs->manager = manager;
    threadArgs->process = process;
    threadArgs->processTime = processTime;

    modifyRunningThreads( manager, +1 );

    pthread_create( &tempId, NULL, sendProcessToInterruptThread, (void *)threadArgs );
}

/*
Name: sendProcessToInterruptThread
Process: store the process in an interruptNode, 
         wait for the specified time, 
         stores the node in the manager. 
         After, decrement the count of running threads
*/
void *sendProcessToInterruptThread( void *voidArgs )
{
    InterruptThreadArgs *threadArgs = (InterruptThreadArgs *)voidArgs;
    InterruptNode *nodeToPush;
    nodeToPush = createInterruptNode( threadArgs->process );

    runTimer( threadArgs->processTime );
    pushInterrupt( threadArgs->manager, nodeToPush );
    modifyRunningThreads( threadArgs->manager, -1 );

    free( voidArgs );
    return NULL;
}

/*
Name: waitingForProcesses
Process: returns if threads are being ran
*/
bool waitingForProcesses( InterruptManager *manager )
{
    return manager->runningThreads > 0;
}




