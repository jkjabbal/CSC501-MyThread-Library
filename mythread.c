#include<stdio.h>
#include<ucontext.h>
#include"mythread.h"
#include"queue.h"
#include<stdlib.h>
#include<string.h>
_MyThread rootthread,thread1,contextmain,exitme,runme;
ucontext_t *rootcontext;
ucontext_t *me;
ucontext_t *maincontext=NULL;
_MyThread ready[200];
_MyThread blocked[200];
int tid = 1;

/*
//Init the Main Thread.Always has a ThreadID = 1.
//params : 1. startfunc() 2. number of arguments
*/
void MyThreadInit(void(*func)(void*),void *args){
getcontext(&thread1.context);
thread1.context.uc_link=0;
thread1.context.uc_stack.ss_sp=malloc(64000);
thread1.context.uc_stack.ss_size=64000;
thread1.context.uc_stack.ss_flags=0;
thread1.parent=0;
thread1.tid=1;
makecontext(&thread1.context,(void(*)())func,1,args);
rootthread=thread1;
rootcontext=&rootthread.context;
getcontext(&rootthread.context);
swapcontext(&rootthread.context,&thread1.context);
}


/*
//Create New Threads.Only add them to the ready queue.
*/
MyThread MyThreadCreate(void(*func)(void*),void *args){
tid++;
_MyThread threadcurrent,threadnew;
getcontext(&threadcurrent.context);
getcontext(&threadnew.context);
threadnew.tid=tid;
threadnew.context.uc_link=0;
threadnew.context.uc_stack.ss_sp=malloc(64000);
threadnew.context.uc_stack.ss_size=64000;
threadnew.context.uc_stack.ss_flags=0;
threadnew.parent=thread1.tid;
threadnew.wait_index=0;
thread1.child[thread1.child_index]=threadnew.tid;
thread1.child_index++;
makecontext(&threadnew.context,(void(*)())func,1,args);
enqueue(0,threadnew,ready);
MyThread *thread = (MyThread*)&threadnew;
return thread;
}

/*
//Yield to the next thread in ready queue. Enqueue the current in ready.
*/
void MyThreadYield(){
_MyThread threadcurrent,threadnew;
threadcurrent = thread1;
getcontext(&threadcurrent.context);
threadnew = thread1;
threadnew = dequeue(0,ready);
if(threadnew.tid != 0){
enqueue(0,threadcurrent,ready);
thread1=threadnew;
swapcontext(&threadcurrent.context,&threadnew.context);
}
else{
thread1=threadcurrent;
}
}


/*
//Exit the current thread
*/
void MyThreadExit(){
_MyThread threadnew,threadcurrent,exitreturn;
threadcurrent=thread1;
getcontext(&threadcurrent.context);
exitreturn = threadcurrent;

if(threadcurrent.tid != 1){
int i;
int j;
for(i=0;blocked[i].tid != 0;i++){
for(j=0;j<blocked[i].wait_index;j++){
if(blocked[i].waiting_on_children[j] == threadcurrent.tid && 0 != threadcurrent.tid ){
enqueue(0,blocked[i],ready);
 blocked[i].tid=0;
 blocked[i].waiting_on_children[j] = 0;
 blocked[i].wait_index=blocked[i].wait_index-1;
}
else{
break;
}
}
}

threadcurrent.tid=0;
threadcurrent.parent=0;

threadnew = dequeue(0,ready);
if(threadnew.tid != 0){
thread1=threadnew;
setcontext(&threadnew.context);
}
else{
setcontext(maincontext);
}
}
else{
threadnew = dequeue(0,ready);
if(threadnew.tid != 0){
setcontext(&threadnew.context);
}
else{
setcontext(rootcontext);
}
}
}

/*
//Block the specified thread and run next from queue.
*/
int MyThreadJoin(MyThread thread){
_MyThread *threadjoin = (_MyThread*)thread;
_MyThread threadcurrent,threadnew;
if(thread1.tid != threadjoin->parent){
return -1;
}
threadcurrent = thread1;
getcontext(&threadcurrent.context);
threadnew = dequeue(0,ready);
if(threadnew.tid != 0){
threadcurrent.waiting_on_children[threadcurrent.wait_index]=threadjoin->tid;
threadcurrent.wait_index++;
enqueue(1,threadcurrent,blocked);
thread1=threadnew;
getcontext(&exitme.context);
swapcontext(&exitme.context,&threadnew.context);
}
maincontext=&threadcurrent.context;
return 0;
}

/*
//Make the parent wait on all its children.
*/
void MyThreadJoinAll(){
_MyThread threadcurrent,threadnew,joinall;
threadcurrent=thread1;
getcontext(&threadcurrent.context);
int k;
for(k=0;k<threadcurrent.child_index;k++){
threadcurrent.waiting_on_children[threadcurrent.wait_index]=threadcurrent.child[k];
threadcurrent.wait_index++;
}
threadnew = dequeue(0,ready);
if(threadnew.tid == 0){
thread1=threadcurrent;
}
else{
enqueue(1,threadcurrent,blocked);
thread1=threadnew;
getcontext(&joinall.context);
swapcontext(&joinall.context,&threadnew.context);
}
maincontext=&threadcurrent.context;
}

MySemaphore MySemaphoreInit(int initialValue){
_MySemaphore sem;
if(initialValue < 0){
}
else{
sem.semaphore=initialValue;
}
MySemaphore *semreturn = (MySemaphore*)&sem;
return semreturn;
}

void MySemaphoreWait(MySemaphore sem){
_MySemaphore *waitsem = (_MySemaphore*)sem;
waitsem->semaphore--;
_MyThread threadcurrent,threadnew;
threadcurrent = thread1;
getcontext(&threadcurrent.context);
if(waitsem->semaphore < 0){
enqueue(2,threadcurrent,waitsem->semaphore_queue);
threadnew = dequeue(0,ready);
if(threadnew.tid != 0){
thread1=threadnew;
swapcontext(&threadcurrent.context,&threadnew.context);
}
else{
setcontext(maincontext);
}
}
}

void MySemaphoreSignal(MySemaphore sem){
_MySemaphore *signalsem = (_MySemaphore*)sem;
signalsem->semaphore++;
_MyThread threadcurrent,threadnew;
threadcurrent=thread1;
if(signalsem->semaphore <= 0){
threadnew = dequeue(2,signalsem->semaphore_queue);
enqueue(0,threadnew,ready);
}
}

int MySemaphoreDestroy(MySemaphore sem){
_MySemaphore *destroysem = (_MySemaphore*)sem;
if(destroysem->semaphore == 0){
destroysem->semaphore = -100; 
return 0;
}
return -1;
}
