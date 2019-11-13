#include "linkList.h"

void pushToList(node **root,account acc)
{
    node* newNode =(node*)malloc(sizeof(node));
    //if(new == NULL){printf("Malloc Error\n");exit(EXIT_FAILURE);}
    newNode->acc = acc;
    newNode->next=*root;
    *root = newNode;
}


void deleteElement(node **root,char *userName)
{
    node *temp,*temp2;
    if(strcmp((*root)->acc.userName,userName)==0)
    {
        temp=*root;
        *root=(*root)->next;
        free(temp);
    }
    else
    {
        temp=*root;temp2=temp->next;
        while(temp2!=NULL)
        {
            if(strcmp(temp2->acc.userName,userName)==0)
            {
                temp->next=temp2->next;
                free(temp2);
            }
            temp=temp->next;
            temp2=temp2->next;
        }
    }
}

void showList(node *root)
{
    while(root!=NULL)
    {
        printf("%-40s %-20s %-5d\n",root->acc.userName,root->acc.password, root->acc.status);
        root=root->next;
    }

}

void freeList(node* head)
{
   node* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }
}
