/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
int status1;
int status2;
int status3;
int status4;

int thread3(void* arg)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 3;
}

int thread2(void* arg)
{
	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 3;
}

int thread1(void* arg)
{
	uthread_create(thread2, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	return 1;
}

int main(void)
{
	uthread_join(uthread_create(thread1, NULL), &status1);
	printf("status1: %d\n",status1 );

	return 0;
}
