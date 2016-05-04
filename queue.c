#include<stdio.h>
#include<ucontext.h>
#include<stdlib.h>
#include"queue.h"
#define THREAD_QUEUE_SIZE 200

static int readyrear = -1;
static int readyfront = -1;

static int blockedrear = -1;
static int blockedfront = -1;

static int semrear = -1;
static int semfront = -1;

void enqueue(int queueID,_MyThread context,_MyThread queue[]){
if(queueID == 0){ // for Ready Queue
if(readyfront == 0 && readyrear == THREAD_QUEUE_SIZE - 1){
}
else if(readyfront == -1 && readyrear == -1){
readyfront =0;
readyrear =0;
queue[readyrear]=context;
}
else if(readyrear == THREAD_QUEUE_SIZE -1 && readyfront !=0){
readyrear = 0;
queue[readyrear] = context;
}
else {
readyrear = readyrear +1;
queue[readyrear] = context;
}
}
else if(queueID == 1){ // for Blocked Queue
if(blockedfront == 0 && blockedrear == THREAD_QUEUE_SIZE - 1){
}
else if(blockedfront == -1 && blockedrear == -1){
blockedfront =0;
blockedrear =0;
queue[blockedrear]=context;
}
else if(blockedrear == THREAD_QUEUE_SIZE -1 && blockedfront !=0){
blockedrear = 0;
queue[blockedrear] = context;
}
else {
blockedrear = blockedrear +1;
queue[blockedrear] = context;
}
}
else if(queueID == 2){
if(semfront == 0 && semrear == THREAD_QUEUE_SIZE - 1){
}
else if(semfront == -1 && semrear == -1){
semfront =0;
semrear =0;
queue[semrear]=context;
}
else if(semrear == THREAD_QUEUE_SIZE -1 && semfront !=0){
semrear = 0;
queue[semrear] = context;
}
else {
semrear = semrear +1;
queue[semrear] = context;
}
}
}

_MyThread dequeue(int queueID,_MyThread queue[]){

_MyThread thread;
if(queueID == 0){ // for Ready Queue
 if(readyfront == -1){
 thread.tid = 0;
 return thread;
 }
 thread = queue[readyfront];
 if(readyfront == readyrear){
 readyfront = -1;
 readyrear = -1;
 }
 else{
 if(readyfront == THREAD_QUEUE_SIZE -1){
 readyfront = 0;
 }
 else {
 readyfront = readyfront + 1;
 }
 }
return thread;
}
else if(queueID == 1){ // for Blocked Queue
if(blockedfront == -1){
 thread.tid=0;
 return thread;
 }
 
 thread = queue[blockedfront];
 if(blockedfront == blockedrear){
 blockedfront = -1;
 blockedrear = -1;
 }
 else{
 if(blockedfront == THREAD_QUEUE_SIZE -1){
 blockedfront = 0;
 }
 else {
 blockedfront = blockedfront + 1;
 }
 }
return thread;
}
else if(queueID == 2){ // for Semaphore Queue
if(semfront == -1){
 thread.tid=0;
 return thread;
 }

 thread = queue[semfront];
 if(semfront == semrear){
 semfront = -1;
 semrear = -1;
 }
 else{
 if(semfront == THREAD_QUEUE_SIZE -1){
 semfront = 0;
 }
 else {
 semfront = semfront + 1;
 }
 }
return thread;
} 
}
