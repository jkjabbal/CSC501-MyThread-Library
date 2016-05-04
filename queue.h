#include<stdio.h>
#include<ucontext.h>
#include<stdlib.h>
#define THREAD_QUEUE_SIZE 200

typedef struct s_MyThread{
int tid;
ucontext_t context;
int parent;
int waiting_on_children[200];
int child[200];
int num_children;
int wait_index;
int child_index;
}_MyThread;

typedef struct s_MySemaphore{
int semaphore;
_MyThread semaphore_queue[200];
}_MySemaphore;

void enqueue(int queueID,_MyThread context,_MyThread queue[]);

_MyThread dequeue(int queueID,_MyThread queue[]);
