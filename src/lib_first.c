#include <stdio.h>
#include <stdlib.h>
#include "../include/linkedlist.h"

//Initialize List
void initList(LinkedList * lp) {
	lp->head = NULL;
	lp->tail = NULL;
}

//Create node and return reference of it.
Node* createNode(void* item) {
	Node* nNode;

	nNode = (Node *) malloc(sizeof(Node));
	if (nNode == NULL) {
		return NULL;
	}

	nNode->item = item;
	nNode->next = NULL;

	return nNode;
}


//Add new item at the end of list.
int insertAtTail(LinkedList * lp, void* item) {
	Node* node;
	node = createNode((void*)item);
	if (node == NULL) {
		return 1;
	}

	//if list is empty.
	if(lp->head == NULL) {
		lp->head = node;
		lp->tail = node;
	} else {
		lp->tail->next  = node;
		lp->tail 	= lp->tail->next;
	}	
	return 0;	
}


//Add new item at beginning of the list.
int insertAtHead(LinkedList* lp, void* item) {
	Node* node;
	node = createNode(item);
	if (node == NULL) {
		return 1;
	}


	//if list is empty.
	if(lp->head == NULL) {
		lp->head = node;
		lp->tail = node;
	} else {
		node->next 	= lp->head;
		lp->head 	= node;
	}		
	return 0;
}

//Add new item at beginning of the list.
int insertAtIndex(LinkedList * list, int index, void* item) {
	Node* to_insert;
	to_insert = createNode(item);
	if (to_insert == NULL) {
		return 0;
	}

	int i = 0;
	Node* prev;
	Node* node = list->head;
	while (node != NULL) {
		if (i == index) {
			prev->next = to_insert;
			to_insert->next = node;
			return 0;
		} else if (i > index) {
			return 0;
		} else {
			i++;
			prev = node;
			node = node->next;
		}
	}	
	return	0; 
}

void* removeAtIndex(LinkedList * list, int index) {
  int i = 0;
  Node* prev;
  Node* node = list->head;
  while (node != NULL) {
    if (i == index) {
      prev->next = node->next;
			void* item = node->item;
			free(node);
    	return item;
    } else if (i > index) {
      	// List is too short
		return NULL;
    } else {
      i++;
      prev = node;
      node = node->next;
    }
  }	
}

//Delete item from Start of list.
void* removeHead(LinkedList * lp) {
	void* item;

	if(lp->head == NULL) {	
		// List is Empty	
		return NULL;
	} else {
		item = lp->head->item;
		Node* old_head = lp->head;
		lp->head = lp->head->next;	
		free(old_head);	
	}	
	return item;
}

//Delete item from the end of list.
void* removeTail(LinkedList * lp) {
	Node * temp;
	int i = 0;

	void* item;

	if(lp->tail == NULL) {	
		// List is Empty	
		return NULL;
	}
	else {
		temp = lp->head;

		// Iterate to the end of the list
		while(temp->next != lp->tail) { 
			temp = temp->next;
		}

		item = lp->tail->item;

		Node* old_tail = lp->tail;
		lp->tail = temp;
		lp->tail->next = NULL;	
		free(old_tail);	
	}	
	return item;
}

void* itemAtIndex(LinkedList* list, int index) {
	int i = 0;
	Node* node = list->head;
	while (node != NULL) {
		if (i == index) {
			return node->item;
		} else if (i > index) {
			return NULL;
		} else {
			i++;
			node = node->next;
		}
	}
}

void printList(LinkedList* list) {
	Node* node;

  // Handle an empty node. Just print a message.
	if(list->head == NULL) {
		printf("\nEmpty List");
		return;
	}
	
  // Start with the head.
	node = (Node*) list->head;

	printf("\nList: \n\n\t"); 
	while(node != NULL) {
		printf("[ %x ]", node->item);

    // Move to the next node
		node = (Node*) node->next;

		if(node !=NULL) {
			printf("-->");
    }
	}
	printf("\n\n");
}

// Returns the size of the list, measured in nodes.
int size(LinkedList* list_pointer){
	Node* current = list_pointer->head;
	int count = 0;
	while(current != NULL){
		count++; //add to count
		current = (Node*) current->next; //current = current->next will eventually make current = null, therefore change while(current->next != NULL) to while(current != NULL)
	}
	return count;
}

// Return true if the list contains the given item at least once, false otherwise.
bool contains(LinkedList* list_pointer, void* item){
	Node* current = list_pointer->head;
	while(current != NULL){ //only works with integers tho
		if(current->item == item){
			return true;
		}
		current = (Node*) current->next;
	}
	return false;	
}