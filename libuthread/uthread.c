#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"
#include "context.h"
#include "preempt.h"
#include "uthread.h"

unsigned short globalCount = 0; //global count for determining TID
queue_t theQ; //global queue to store our ready threads
queue_t zombieQ; //global queue to store zombie threads
queue_t blockedQ; //global queue to store blocked threads

int returnValue[64]; //store all return values for each tid, index by their TID


typedef struct TCB
{
	uthread_t TID;
	uthread_ctx_t threadContent;
	char* status;
	int returnValue;
	uthread_t blockedBy;
	int exitValue;

}TCB;

TCB* runningThread;

// initializes threading
// creates thread for main process
// initializes the queues
void threading_init(uthread_func_t func, void *arg)
{
	theQ = queue_create();
	zombieQ = queue_create();
	blockedQ = queue_create();
	
	TCB* mainprocess = (TCB*)malloc(sizeof(TCB));
	
	mainprocess->TID = globalCount;
	mainprocess->status = "running";
	runningThread = mainprocess; // set running thread to main process
}


int uthread_create(uthread_func_t func, void *arg)
{
	if (globalCount == 0){
		threading_init(func,arg);
		preempt_disable();
		globalCount++;
		preempt_enable();
	}

	/*
	/ creating the new TCB object
	*/
	TCB* newTCB;
	newTCB = (TCB*) malloc(sizeof(TCB));
	void* stack;
	stack = uthread_ctx_alloc_stack();
	//newTCB->threadContent = (uthread_ctx_t) malloc(sizeof(uthread_ctx_t));
	uthread_ctx_init(&(newTCB->threadContent),stack,func,arg);
	newTCB->status = "ready";

	preempt_disable();
	newTCB->TID = globalCount;
	queue_enqueue(theQ,(void*)newTCB);
	globalCount++;
	preempt_enable();

	return newTCB->TID;
}

void uthread_scheduleNewThread(void)
{
	TCB* next;
	TCB* temp;
	int queueStatus;

	preempt_disable();
	queueStatus = queue_dequeue(theQ,(void**)&next);
	preempt_enable();
	if(queueStatus != -1){
		next->status = "running";
		temp = runningThread;
		runningThread = next;
		uthread_ctx_switch(&(temp->threadContent),&(next->threadContent));
		
	}
}

void uthread_yield(void)
{	
	// preempting a yield call in order to yield to the next thread wastes time
	preempt_disable();
	runningThread->status = "ready";
	queue_enqueue(theQ,(void*)runningThread);
	uthread_scheduleNewThread();
	preempt_enable();
}

uthread_t uthread_self(void)
{
	return runningThread->TID;
}

static int find_blockedID(queue_t q, void *data, void *arg)
{
	TCB *newData = (TCB*)data;
	int* a = (int*)&(newData->blockedBy);
    	int* match = (int*)arg;
    	if(queue_length(q) == 0){
    		return 0;
    	}

	
    	if (*a == *match){
        	return 1;
    	}
    	return 0;

}

void uthread_exit(int retval)
{
	runningThread->exitValue = retval;
	runningThread->status = "zombie";

	preempt_disable();
	queue_enqueue(zombieQ,(void*)runningThread);
	preempt_enable();

	//check if exiting thread is blocking any other gread	

	TCB* find; //find the thread that is currently being blocked by the running thread
	find = (TCB*)malloc(sizeof(TCB));
	int newTID = (int)runningThread->TID;

	
	queue_iterate(blockedQ,find_blockedID,(void*)&newTID,(void**)&find);

	if(find->blockedBy == runningThread->TID){
		preempt_disable();
		queue_delete(blockedQ,(void*)find);
		preempt_enable();
	}
	

	if(find->blockedBy == runningThread->TID){
		find->status = "ready";
		preempt_disable();
		queue_enqueue(theQ,(void*)find);
		preempt_enable();
		uthread_scheduleNewThread();
	}
	else{
		uthread_scheduleNewThread();
	}
}



static int find_threadID(queue_t q, void *data, void *arg)
{
	TCB *newData = (TCB*)data;
	int* a = (int*)&(newData->TID);
    	int* match = (int*)arg;
	if(queue_length(q) == 0){
		return 0;
	}


	if (*a == *match){
    	return 1;
	}
	return 0;

}


int uthread_join(uthread_t tid, int *retval) //thread waits for thread with tid
{
	if(runningThread->TID == tid || tid < 0){
		return -1;
	}

	int newTID = (int)tid;
	//find the TCB in theQ

	TCB* find; 
	TCB* find2; //see if newID is a ready thread
	TCB* find3;
	TCB* find4;

	find = (TCB*)malloc(sizeof(TCB));
	find2 = (TCB*)malloc(sizeof(TCB));
	find3 = (TCB*)malloc(sizeof(TCB));
	find4 = (TCB*)malloc(sizeof(TCB));

	queue_iterate(theQ,find_threadID,(void*)&newTID,(void**)&find2);
	queue_iterate(blockedQ,find_blockedID,(void*)&newTID,(void**)&find3);
	queue_iterate(blockedQ,find_threadID,(void*)&newTID,(void**)&find4);
	queue_iterate(zombieQ,find_threadID,(void*)&newTID,(void**)&find); 

	if(find3->blockedBy == newTID || find4->TID == newTID){ //joining with something that is being blocked or blocked
			return -1;
		}

	if(find->TID == newTID){ //thread found in zombieQ, so thread has exited
		runningThread->status = "ready";
		if(retval != NULL){
			*retval = find->exitValue;
		}
		preempt_disable();
		queue_delete(zombieQ,(void*)find);
		preempt_enable();
		uthread_scheduleNewThread();
	}
	else{ //thread has not exited.
		if(find2->status == NULL && find->status == NULL && find3->status == NULL){
			return -1;
		}

		runningThread->status = "blocked";
		runningThread->blockedBy = tid;

		preempt_disable();
		queue_enqueue(blockedQ,(void*)runningThread);
		preempt_enable();
		uthread_scheduleNewThread();
		if(find2->exitValue > 0){
			*retval = find2->exitValue;
		}
	}
	return 0;	
}


