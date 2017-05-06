ECS150-P2:Thread Library

For queue testing:
TA said test like: 

int* ptr1;
queue_dequeue(aQ,(void**)&ptr1);
printf("address after dereference: %d\n",((int*)ptr1));
/////////////////////////////////////////////////////////////////////////////
queue is now working, ran test test_queue.c in root directory and everything prints out correctly

///////////////////////////////////////////////////////////////////////////
malloc empty ctx_object to save main process
malloc and init another ctx_object for first thread
switch from main to first thread
enqueue main thread
switch will automatically save main into tcb object.


//////////////////////////////////////////////////////////////////////////
need to test uthread_join and verfiy implementation for exit
