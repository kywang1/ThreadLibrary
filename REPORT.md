# Project 2 User Thread Library

#### General Information
  - Authors: Kyle Wang and Patrick Ong
  - Source Files: preempt.c, uthread.c, and preempt.c
  - Test Files: queue_test.c, queue_test2.c, uthread_hello.c, uthread_yeild.c, 
  uthread_testThreads.c,test_preempt.c

# High Level Design Choices 
Our thread library is split into three seperate
files, preempt.c, uthread.c, and preempt.c. There are also multiple test files
that we wrote to test our thread library. As stated in the instruction, we did
not modify any of the source code in any of the eader files.

In our queue.c we chose to implement our generic queue using a linked list
implemenation. We created two structs to implement our queue, typedef struct
queue and typedef struct queueNode.The head will be the oldest element in the
queue, the element we wil dequeue, while the tail contains the newest element in
the queue. In the queueNode, we have two pointers, previous and next that keep
tracks of the neighbors of each queueNode. The dataPtr is a void pointer that
will be responsible for storin any data type that we are passing to our queue.

```
typedef struct queueNode{
	...
}queueNode;

typedef struct queue {
	...
}Queue;
```

In our uthread.c file, we created on struct, typedef struct TCB. This struct
will hold the information of every single thread that we create. Using the queue
that we implemented, we store our threads in this data structure, and schedule
our threads according to a first in first out (FIFO) system. The global
variables that we declared help us keep tract of our threads. We have three
queues that keep track of our threads in three different states: ready, zombie,
and blocked. To keep track of our running thread, we use a global pointer to
keep track of the single thread. Implenetation details of the functions will be
in implementation details.

```
TCB* runningThread;
typedef struct TCB{
	...
}TCB;
```
# **Implementation Details**

### Phase 1: queue API
#### queue_create() and queue_destory()
The implementation of queue_create() required the creation of a pointer to the
Queue and properly using malloc to allocate the necessary memory for it. We than
return the pointer to the Queue that we created. In queue_destory(queue_t queue)
we de-allocate the memory of the queue by calling free on the queue and its head
and tail pointer.

#### int queue_enqueue(queue_t queue, void *data) In queue_enqueue(), we
allocate a queueNode* newNode for the pieces of data that we are adding to our
queue. If the enqueued element is the first element of the queue, we set the
head and tail to point to it. If the enqueued data is not the first element of
the queue, we set the dataPtr in our newly created data to point at the enqueued
data. Then we set the newNode next pointer to be NULL since this is the newest
item. The newNode's previous pointer will be the tail of the queue before the
the insertion the the newNode. The next element of the tail before the insertion
of the newNode will be the newNode. Finally we set the tail of the queue to be
the new Node. At the end we increment the length of our queue.

#### queue_dequeue(queue_t queue, void **data)
In this function, the most important check is to make sure that the queue length
is greater than 0, so we will have atleast one element to remove from our queue.
The void **data is where we wil be storing the data that we dequeue from our
queue. We do this by:

```
*data = queue->head->dataPtr;
```

This will do a one level dereference on data and set it to point at the queue
head dataPtr, since the head contains the oldest element in our queue. There is
one case that we have to consider after this, we have to check the length of the
queue after dequeue. If the length becomes zero, we set the head and the tail of
the queue to point to NULL.

#### int queue_delete(queue_t queue, void *data)

When performing delete, we are trying to remove a specific from a queue and
return it to the user. We must determine if such an item exists in our queue. We
keep a boolean variable found to tell us if we found a match. To perform this we
have a for loop that searches through our queue starting at the head:

```
queueNode* nodePtr;
nodePtr = (queueNode*) malloc(sizeof(queueNode));
nodePtr = queue->head;
for(nodePtr = queue->head; nodePtr!= NULL; nodePtr = nodePtr->next){
    ...
}
```

We have an if statement to check if the dataPtr in the node is equivalent to the
void *data passed in from the function argument. If there is a match we will
have our dataPtr pointing to the data argument and we break our of our for loop.
If there is only one element in the queue we must set the head and tail pointer
to NULL in the queue. If there contains more than one element in the queue,
there are still three cases to consider.

1. Deletion from the head of the queue

2. Deletion from the tail of the queue

3. All other deletions

#### queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
In our iterate function, we pass perform a function operations on every element
in our queue. We use a for loop to access and perform operations on the elements
in our queue:

```
for(...){
		returnStatus = func(queue,nodePtr->dataPtr,arg);
		...
}
```
If data is found:

```
if(returnStatus == 1)
	if(data!= NULL)
		*data = nodePtr->dataPtr; //set found data
		return 0;
```

##### int queue_length(queue_t queue)
In here we simply return the queue length that we were keeping track throughout
all of our functions.

### Phase 2: uthread API
##### int uthread_create(uthread_func_t func, void *arg)
We initializes the data for our queue and returns a pointer to it

##### void uthread_yield(void)
When performing yield, the running thread is giving up its processor power to
another thread. The runningThread is set to a ready status, and enqueued into
our ready queue. We than call our uthread_scheduleNewThread() function. This
function provides the swapping of context between two threads:

```
void uthread_scheduleNewThread(void){
	...
}
```

This function takes a thread from our ready queue, sets its status to running,
points our runningTHread pointer at it, and perform a context switch with the
previous running thread, stored in temp, and the next running thread. This
function well be called in our other functions as well.

##### void uthread_exit(int retval)
When a thread exits, we set the exitValue variable of our currently running
thread to the retval we receive when when our thread terminates. The running
thread status is set to zombie and is enqueued in the zombie queue. A check is
required to see if the running thread is blocking any other threads. We do this
by calling queue_iterate() on our blockedQ, which contains all the blocked
threads:
```
queue_iterate(blockedQ,find_blockedID,(void*)&newTID,(void**)&find);
```

Our find_blockedID function performs a check to see if our current thread TID,
newTID in the function call, matches with any of the blockedBy member variable
in our blockedQ. If the exiting thread is blocking any threads, we delete the
thread being blocked from our blockQ and set it to ready, than enqueue into our
readyQ. We than schedule a new thread. If there was no match, we just schedule a
new thread.

### Phase 3: uthread_join()
##### int uthread_join(uthread_t tid, int *retval)

In this function, the currently running is trying to collect another thread,
base on its tid. We allocated 4 TCB pointers in the beginning to search through
the three queues that we have.

The find_threadID() function that we use performs a search for the tid, passed
into uthread_join(), on all of our queues, and will return the found element to
our allocated TCB pointers. We also perform queue_iterate on the blockedQ,
because we want to check if the tid that the running thread is trying to join is
blocking another thread or is being blocked. In either of these cases, we cannot
join a thread that is already joining with another thread. The if statement
after checks for this:
```
if(find3->blockedBy == newTID || find4->TID == newTID)
```
There will only be 2 cases afterwards. find will determine if the tid is found
in our zombie queue. If the tid that the running thread is is trying to join is
in the zombie queue:
```
if(find->TID == newTID)
```
If this case is true, we set the retval to be the exit value of the thread that
we are collecting and remove that thread from the zombie queue. We than go and
schedule a new thread. If the thread that the running thread is trying to
collect is not found in the zombieQ, that means this thread has not finished
execution. We set running thread to block, since collection of another thread
has failed, and set the blockedBy member value to the thread that it was trying
to collect. The running thread is enqueued into the blocked queue, and a new
thread is scheduled.


### Phase 4: Preemption
##### void preempt_start(void)
In this function, we initialize the timer and set the alarm handler to handle
the SIGVTALRM signal. This is done by declaring structures of type sigset_t,
sigaction, and itimerval.

The sigaction structure holds a function pointer, a blocked mask, and a set of
flags. We set the sa_handler pointer to point at our alarm_Handler() function,
then call the sigaction() function. The function call associates the SIGVTALRM
signal with the sigaction struct, so that when the process receives a SIGVTALRM
signal, alarm_Handler() is called.

Finally, the itimerval structure contains several variables which determine the
duration until the first alarm and between every subsequent alarm. We set both
of these values to 10 milliseconds, then call the setitimer() function with the
ITIMER_VIRTUAL timer code in order to send the corresponding SIGVTALRM signal
after the specified amount of time.

##### void alarm_Handler(int signum)
This is a simple function. Whenever the process receives a SIGVTALRM signal,
this function is invoked to forcibly switch to the next ready thread. We
accomplish this by simply calling uthread_yield().

##### sigset_t set_create()
This function creates a new sigset_t structure that contains the SIGVTALRM
signal. At first, we tried using a global sigset_t structure so that we would
only have to create it once. However, this was dangerous because the kernel
could overwrite it.

##### void preempt_{enable,disable}()
These functions enable and disable preemption by unblocking and blocking the
SIGVTALRM signal, respectively. We call the set_create() function in order to
create the appropriate sigset_t object. By calling the sigprocmask() function on
this set, we can block or unblock all signals in the set for the entire process
at once. Although the documentation for sigprocmask() warns against its use in
multi-threaded applications, we are implementing user-level threads. This means
that the kernel does not know about a process's threads, and there is only a
single signal mask for the entire process.

# Testing
#### Testing Phase 1: Queue API

In our file, test_queue2.c, we have a sequence of queue operations that we
performed that tested every queue function that we were responsible for
implementing. We also tested our queue with the given test file: test_queue.c.

#### Testing Phase 2: User Thread Library
Before the implemenation of phase 3, we did testing with the given test files:
uthread_hello.c and uthread_yield.c. After building our library we were able to
receive the correct outputs when running our library. However, it was in phase 3
that we realized that we had flaws in our implentation

#### Testing Phase 3: uthread_join()
For manual testing of our threads, we had a file, uthread_testThreads.c, were we
created four threads. In this file we tested our uthread_join() to make sure
that threads could join other threads. We encountered issues were sometimes
threads could not join other threads. This error was due to a problem with our
queue_iterate function (explained in queue_iterate() implementation details).

Phase 3 exposed alot of other minor bugs that we did not catch in phase 2, and
also some design flaws, mostly in exit. These errors were rectified.

One major testing problem that we encountered was that we could not return the
correct exit status. The issue was that we were trying to set the return value
in our exit function. We fixed it by setting the exit value after we return from
our scheduling a new thread in join.

```
uthread_scheduleNewThread();
*retval = find2->exitValue;	
```

#### Testing Phase 4: Preemption

We created test_preempt.c in order to test our implementation of thread
preemption. In it, we start preemption, then create 3 threads. In the first
thread, we disable preemption then reenable preemption immediately, then run an
infinite loop. In the second thread, we simply print out a message and yield to
thread three. In the third thread, we disable preemption again and run another
infinite loop.

From thread 3, we know that preempt_disable() works because it never exits the
infinite loop. Therefore, we know that preempt_enable() works properly in thread
1 because otherwise, we would get stuck in the infinite loop of thread 1 instead
of going on to thread 3.

# Sources Used
- https://www.gnu.org/software/libc/manual/html_mono/libc.html#
- http://www.informit.com/articles/article.aspx?p=23618&seqNum=14
