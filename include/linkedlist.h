#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node{
    void* item;
    struct Node* next;
}Node;

typedef struct LinkedList{ // use listname->head = blah to chenge and stuff
    Node* head;
    Node* tail;
}LinkedList;

// Initialize an empty list
void initList(LinkedList* list_pointer);

// Create node containing item, return reference of it.
Node* createNode(void* item);

// Insert new item at the end of list.
int insertAtTail(LinkedList* list_pointer, void* item);

// Insert item at start of the list.
int insertAtHead(LinkedList* list_pointer, void* item);

// Insert item at a specified index.
int insertAtIndex(LinkedList* list_pointer, int index, void* item);

// Remove item from the end of list and return a reference to it
void* removeTail(LinkedList* list_pointer);

// Remove item from start of list and return a reference to it
void* removeHead(LinkedList* list_pointer);

// Insert item at a specified index and return a reference to it
void* removeAtIndex(LinkedList* list_pointer, int index);

// Return item at index
void* itemAtIndex(LinkedList* list_pointer, int index);

// Return true if the list contains the given item at least once, false otherwise.
bool contains(LinkedList* list_pointer, void* item);

// Returns the size of the list, measured in nodes.
int size(LinkedList* list_pointer);

// Print List
void printList(LinkedList* list_pointer);


#ifdef __cplusplus
}
#endif

#endif /* LINKEDLIST_H */

