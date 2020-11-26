#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Account {
	char username[30];
	char password[30];
	int status;
} Account;

typedef struct Node {
  Account acc;
  struct Node *next;
} Node;

void insertFirst(Node **root, Account acc);
void printLinkedList(Node *root);
void freeList(Node* head);