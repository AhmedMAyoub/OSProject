#include "headers.h"

struct node;

struct node{
    struct process* process;
    struct node* nextNode;
};

struct queue{
    struct node* headPtr;
    struct node* tailPtr;
};



struct queue* initizalize(){
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    q->headPtr = NULL;
    q->tailPtr = NULL;
    return q;
}



bool isEmpty(struct queue* q){
    if(q->headPtr==NULL){
        return true;
    }else{
        return false;
    }
}


void enqueue(struct queue* q , struct process* p){
    struct node* processNode ;
    processNode->process=p;
    processNode->nextNode=NULL;
    if(isEmpty(q)){
        q->headPtr = processNode;
        q->tailPtr = processNode;
        return;
    }
    q->tailPtr->nextNode = processNode;
    q->tailPtr = processNode;
    q->tailPtr->nextNode=q->headPtr;
    return;
}


struct process* dequeue(struct queue* q){
    if(isEmpty(q)){
        return NULL;
    }
    struct process* tempProcess;
    if(q->headPtr==q->tailPtr)
    {
        tempProcess=q->headPtr->process;
        free(q->headPtr);
        q->headPtr=NULL;
        q->tailPtr=NULL;
    }else{
        struct node* temp= q->headPtr;
        tempProcess=temp->process;
        q->headPtr=q->headPtr->nextNode;
        q->tailPtr->nextNode=q->headPtr;
        free(temp);
    }
    return tempProcess;
}


