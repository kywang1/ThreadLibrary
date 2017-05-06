#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"

#define MAX_BLOCK_SIZE unsigned long long int

//queue Node
typedef struct queueNode{
	void *dataPtr;
	struct queueNode *previous; // opposite of next
	struct queueNode *next; // head, then next is second, then next is third, etc
}queueNode;


typedef struct queue {
	/* TODO */
	queueNode* head; // next to be popped
	queueNode* tail; // push here
	int length;
}Queue;

queue_t queue_create(void)
{
	Queue* theQ;
	theQ = (Queue*) malloc(sizeof(Queue)); //size of queue??
	// if theQ == NULL, return theQ
	theQ->length = 0;
	theQ->head = (queueNode*)malloc(sizeof(queueNode));
	theQ->tail = (queueNode*)malloc(sizeof(queueNode));
	return theQ;
}

int queue_destroy(queue_t queue)
{

	if (queue == NULL || queue->length != 0){
		return -1;
	}

	free(queue->head);
	free(queue->tail);
	free(queue);
	
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if(data == NULL || queue == NULL){
		
		return -1;
	}
	queueNode* newNode;
	newNode = (queueNode*)malloc(sizeof(queueNode));


	if(queue->length == 0){
		queue->head = newNode;
		queue->tail = newNode;
		queue->head->dataPtr = data;
		//printf("*data: %d\n",*(int*)queue->head->dataPtr);
	}else{

		newNode->dataPtr = data;
		newNode->next = NULL;
		newNode->previous = queue->tail;
		queue->tail->next = newNode;
		queue->tail = newNode;
		//printf("*data: %d\n",*(int*)queue->tail->dataPtr);
	}

	
	queue->length++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || queue->length == 0){
		return -1;
	}


	*data = queue->head->dataPtr;

	
	queueNode* temp;
	temp = queue->head;
	
	queue->head = queue->head->next;
	//queue->head->previous = NULL;
	queue->length--;
	free(temp);
	if(queue->length == 0){
		queue->head = NULL;
		queue->tail = NULL;
	}
	//printf("*data2: %d\n",*((int*)data));
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL){
		return -1;
	}

	bool found = false; //true if we found the data,false if not
	queueNode* nodePtr;
	nodePtr = (queueNode*) malloc(sizeof(queueNode));
	nodePtr = queue->head;
	for(nodePtr = queue->head; nodePtr!= NULL; nodePtr = nodePtr->next){

		if (nodePtr->dataPtr == data) //found the right data
		{
			if(queue->length == 1){
				//printf("Found: %d\n",*(int*)nodePtr->dataPtr);	
				queue->head = NULL;
				queue->tail = NULL;
				queue->length--;
				free(nodePtr);
				found = true;
			}
			else{
				//printf("Found: %d\n",*(int*)nodePtr->dataPtr);	
				if(nodePtr == queue->head){
					queue->head = queue->head->next;
					queue->head->previous = NULL;
				}
				else if(nodePtr == queue->tail){
					queue->tail = queue->tail->previous;
					queue->tail->next = NULL;
				}
				else{
					nodePtr->previous->next = nodePtr->next;
					nodePtr->next->previous = nodePtr->previous;
				}
				queue->length--;
				free(nodePtr);
				found = true;
			}
			break;

		}
	}

	if(!found){

		return -1;
	}

	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	//printf("Iterate\n");
	
	if(queue == NULL || queue->length == 0 || func == NULL){
		//printf("queue or data is NULL\n");
		return -1;
	}
	

	int returnStatus;
	int i;
	queueNode* nodePtr;
	nodePtr = (queueNode*) malloc(sizeof(queueNode));
	nodePtr = queue->head; 
	
	if(queue->length == 1){
		returnStatus = func(queue,nodePtr->dataPtr,arg);
		if(returnStatus == 1){
			if(data!= NULL){
				*data = nodePtr->dataPtr; //set found data
				return 0;
			}
		}
		else{
			return 0;
		}
	}

	for(i = 0, nodePtr = queue->head; i <= queue-> length && nodePtr!= NULL; nodePtr = nodePtr->next){
		returnStatus = func(queue,nodePtr->dataPtr,arg);
		if(returnStatus == 1){ //found data
			if(data!= NULL){
				*data = nodePtr->dataPtr; //set found data
				return 0;
			}
		}
	}

	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL){
		return -1;
	}
	return queue->length;
}

