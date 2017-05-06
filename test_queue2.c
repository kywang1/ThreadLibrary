#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <queue.h>

/* Callback function that increments items by a certain value */
static int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (intptr_t)arg;

    *a += inc;
   
    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (intptr_t)arg;

    if (*a == match)
        return 1;

    return 0;
}


int main(){
	queue_t aQ = queue_create();
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;
	int e = 5;

	queue_enqueue(aQ,&a);
	queue_enqueue(aQ,&b);
	queue_enqueue(aQ,&c);

	//oid** ptr1 ;
	int* ptr1;
	int* ptr2;
	int* ptr3;

	queue_dequeue(aQ,(void**)&ptr1);
	queue_dequeue(aQ,(void**)&ptr2);
	queue_dequeue(aQ,(void**)&ptr3);


	int status;

	queue_enqueue(aQ,&a);
	queue_enqueue(aQ,&b);
	queue_enqueue(aQ,&c);
	queue_enqueue(aQ,&d);
	queue_enqueue(aQ,&e);

	queue_delete(aQ,&a);
	queue_delete(aQ,&b);
	queue_delete(aQ,&e);

	queue_dequeue(aQ,(void**)&ptr1);
	queue_dequeue(aQ,(void**)&ptr2);
	status = queue_dequeue(aQ,(void**)&ptr3);

	printf("after delete and DQ: %d\n",*ptr1);
	printf("after delete and DQ: %d\n",*ptr2);
	printf("status of DQ on empty Q: %d\n",status);
	
	
	printf("length: %d\n",queue_length(aQ));
	
	queue_enqueue(aQ,&a);
	queue_enqueue(aQ,&b);
	queue_enqueue(aQ,&c);
	queue_enqueue(aQ,&d);
	queue_enqueue(aQ,&e);

	printf("length: %d\n",queue_length(aQ));

	int* ptr;
	queue_iterate(aQ, inc_item, (void*)1, NULL);
	queue_iterate(aQ, find_item, (void*)5, (void**)&ptr);
	
	printf("Found data from iterate find_item: %d\n",*ptr );
	return 0;
}

