/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

//test

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>


int status1;
int status2;
int status3;
int status4;
int status6;
uthread_t one = 1;
uthread_t two = 2;
uthread_t three = 3;
uthread_t four = 4;
uthread_t six = 6;

int hello1(void* arg)
{
	uthread_join(three,&status3);
	printf("Thread1!\n");
	return 0;
}

int hello2(void* arg)
{
	uthread_join(four,&status4);
	printf("Thread2!\n");
	return 2;
}

int hello3(void* arg)
{	
	printf("Thread3!\n");
	
	return 3;
}

int hello4(void* arg)
{
	printf("Thread4!\n");
	return 4;
}


int main(void)
{
	int tid1,tid2,tid3,tid4;
	int temp;

	tid1 = uthread_create(hello1,NULL);
	tid2 = uthread_create(hello2,NULL);
	tid3 = uthread_create(hello3,NULL);
	tid4 = uthread_create(hello4,NULL);
	printf("After thread creations\n");
	//printf("&status2: %p\n",&status2);
	uthread_yield();
	printf("After Yeild\n");
	uthread_join(one,&status1);
	uthread_join(two,&status2);
	 //will not run thread1 b/c main is ahead of thread1 in the ready queue.
	
	
	//uthread_join(four,&status4);
	temp = uthread_join(six,&status6);

	printf("tid1: %d\n", tid1);
	printf("tid2: %d\n", tid2);
	printf("tid3: %d\n", tid3);
	printf("tid4: %d\n", tid4);

	//printf("&status2: %p\n",&status2);
	printf("status1: %d\n",status1);
	printf("status2: %d\n",status2);
	printf("status3: %d\n",status3);
	printf("status4: %d\n",status4);
	printf("return status of no-existing thread: %d\n",temp);

	return 0;
}
