#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "preempt.h"
#include "uthread.h"

// creates the sigset_t object containing SIGVTALRM
sigset_t set_create(){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGVTALRM);
	return set;
}

// yield upon receiving a SIGVTALRM
void alarm_Handler(int signum){
	uthread_yield();
}

// block SIGVTALRM
void preempt_disable(void){
	sigset_t block = set_create();
	sigprocmask(SIG_BLOCK,&block,NULL);
}

// enable SIGVTALRM
void preempt_enable(void){
	sigset_t enable = set_create();
	sigprocmask(SIG_UNBLOCK,&enable,NULL);
}

void preempt_start(void){
	struct sigaction sa;
	struct itimerval timer;

	// call alarm_Handler when signal received
	memset(&sa, 0, sizeof (sa));
	sa.sa_handler = &alarm_Handler;
	sigaction(SIGVTALRM, &sa, NULL);

	// initialize timer to go off every 10 milliseconds
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 10000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 10000;

	// start timer
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

