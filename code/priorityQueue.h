#include "headers.h"

typedef struct pnode { 
    struct process* process; 
    struct pnode* next; 
    int priority; 

  
}Node; 
  
Node* newNode( struct process* proc , int p) 
{ 
    Node* temp = (Node*)malloc(sizeof(Node)); 
    temp->process = proc; 
    temp->priority = p; 
    temp->next = NULL; 
  
    return temp; 
} 

struct process* peek(Node** head) 
{ 
    return (*head)->process; 
} 
  

void pop(Node** head) 
{ 
    Node* temp = *head; 
    (*head) = (*head)->next; 
    free(temp); 
} 

void push(Node** head, struct process* proc , int p) 
{ 
    Node* start = (*head); 
    Node* temp = newNode( proc, p); 
    if ((*head)->priority > p) { 

        temp->next = *head; 
        (*head) = temp; 
    } 
    else { 
        while (start->next != NULL && 
               start->next->priority < p) { 
            start = start->next; 
        } 
        temp->next = start->next; 
        start->next = temp; 
    } 
} 

int isEmpty(Node** head) 
{ 
    return (*head) == NULL; 
}