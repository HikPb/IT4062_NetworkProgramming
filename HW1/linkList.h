#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct account
{
  int status;
  char userName[50];
  char password[30];
}account;
typedef struct node{
  account acc;
  int statusLogin;
  struct node *next;
} node;

void pushToList(node **root,account acc);
void deleteElement(node **root,char *userName);
void showList(node *root);
void freeList(node* head);
