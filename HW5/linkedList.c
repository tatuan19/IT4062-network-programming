#include "linkedList.h"

void insertFirst(Node **root, Account acc) {
    Node* newNode =(Node*)malloc(sizeof(Node));
    newNode->acc = acc;
    newNode->next=*root;
    *root = newNode;
}

void printLinkedList(Node *root) {
    while(root!=NULL) {
        printf("%s\t%s\t%d\n",root->acc.username,root->acc.password, root->acc.status);
        root=root->next;
    }
}

void freeList(Node* head) {
   Node* tmp;
   while (head != NULL) {
       tmp = head;
       head = head->next;
       free(tmp);
    }
}