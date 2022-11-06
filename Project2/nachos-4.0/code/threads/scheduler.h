// scheduler.h 
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"

// The following class defines the scheduler/dispatcher abstraction -- 
// the data structures and operations needed to keep track of which 
// thread is running, and which threads are ready but not running.

enum SchedulerType {
        RR,     // Round Robin
        SJF,
        Priority,
		FIFO	//-----------------------Homework------------------------
};

class Scheduler {
  public:
	Scheduler();		// Initialize list of ready threads 
	~Scheduler();		// De-allocate ready list


	/*-----------------------Homework------------------------*/
	Scheduler(SchedulerType type);		// Initialize list of ready threads
	SchedulerType getSchedulerType() {return schedulerType;}
    void setSchedulerType(SchedulerType t) {schedulerType = t;}
	/*-----------------------Homework------------------------*/


	void ReadyToRun(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();	// Dequeue first thread on the ready 
								// list, if any, and return thread.
	void Run(Thread* nextThread, bool finishing);	// Cause nextThread to start running
	void CheckToBeDestroyed();	// Check if thread that had been
    							// running needs to be deleted
	void Print();				// Print contents of ready list
    
    // SelfTest for scheduler is implemented in class Thread
    
  private:
	SchedulerType schedulerType;
	List<Thread *> *readyList;	// queue of threads that are ready to run,
					// but not running
	Thread *toBeDestroyed;		// finishing thread to be destroyed
    					// by the next thread that runs
};


/* ********************************************* 
	Self-defined
********************************************* */
int SJFCompare(Thread *a, Thread *b)
{
    if(a->getBurstTime() == b->getBurstTime())
        return 0;
    return a->getBurstTime() > b->getBurstTime() ? 1 : -1;
}
int PriorityCompare(Thread *a, Thread *b)
{
    if(a->getPriority() == b->getPriority())
        return 0;
    return a->getPriority() > b->getPriority() ? 1 : -1;
}
int FIFOCompare(Thread *a, Thread *b)
{
    return 1;
}
//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------
Scheduler::Scheduler()
{
    Scheduler(RR);
}
Scheduler::Scheduler(SchedulerType type)
{
    schedulerType = type;
    switch(schedulerType)
	{
		case RR:
			readyList = new List<Thread *>;
			break;
		case SJF:
			readyList = new SortedList<Thread *>(SJFCompare);
			break;
		case Priority:
			readyList = new SortedList<Thread *>(PriorityCompare);
			break;
		case FIFO:
			readyList = new SortedList<Thread *>(FIFOCompare);
    }
    toBeDestroyed = NULL;
} 

#endif // SCHEDULER_H
