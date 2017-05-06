#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <uthread.h>
#include <preempt.h>


int thread1(void* arg)
{
	printf("Thread 1 start\n");
	preempt_disable();
	preempt_enable();
	while(1);
	printf("Thread 1 end\n");
	uthread_yield();
	return 1;
}

int thread2(void* arg)
{	
	printf("Thread 2\n");
	uthread_yield();
	return 2;
}

int thread3(void* arg)
{
	printf("This is thread 3. It is expected to loop infinitely.\n");
	printf("Please end the process if this has been verified.\n");
	preempt_disable();
	while(1);
	printf("Thread 3 end\n");
	return 3;
}

int main()
{
	printf("Main start\n");
	preempt_start();
	uthread_create(thread1,NULL);
	uthread_create(thread2,NULL);
	uthread_create(thread3,NULL);
	
	uthread_yield();
	printf("Main end\n");
	return 0;
}
