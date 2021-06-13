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
    return;
}


struct process* dequeue(struct queue* q){
    if(isEmpty(q)){
        return NULL;
    }
    struct node * tempNode = q->headPtr;
    q->headPtr = q->headPtr->nextNode;
    if(q->headPtr==NULL){
        q->tailPtr = NULL ;
    }
    struct process* tempProcess = tempNode->process;
    free(tempNode);
    return tempProcess;
}


