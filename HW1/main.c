#include <stdio.h>
#include <string.h>
#include "linkList.h"

FILE *f;
void readFile(node **root);
void registerAcc(node **root);
void login(node *root);
void search(node *root);
void writeFile(node *root);
void logout(node *root);

int main(){
    //node *root = (node*)malloc(sizeof(node));
    node * root =NULL;
    int ch;
    readFile(&root);
    do{
        printf("USER MANAGEMENT PROGRAM \n");
        printf("---------------------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Signout\n");
        printf("Your choice (1-4, other to quit):");
        scanf("%d",&ch);
        if(ch==1){
            registerAcc(&root);
            showList(root);
        }else if(ch==2){
            login(root);
        }else if(ch==3){
            search(root);
        }else if(ch==4){
            logout(root);
        }
    }while(ch>0&&ch<5);
    freeList(root);
    return 0;
}


void registerAcc(node **root)
{
    char name[50],pass[30];
    node* r= *root;
    int check=0;
    printf("----------Register----------\n");
    printf("Username: ");scanf("%s",name);
    while (r!=NULL)
    {
        if(strcmp(r->acc.userName,name)==0){check =1;}
        r =r->next;
    }
    if(check==1) {printf("Account exsisted!!!\n");}
    else{
        printf("\nPassword: ");scanf("%s",pass);
        account a;
        strcpy(a.userName,name);
        strcpy(a.password,pass);
        a.status=1;
        pushToList(root,a);
        writeFile(*root);
        printf("Successful registration!!\n");
    }
}

void login(node *root){
    char name[50],pass[30];
    int check=0;
    int check2=0;
    node *r=root;
    printf("-------------Login------------\n");
    printf("Username: ");scanf("%s",name);
    while (r!=NULL)
    {
        if(strcmp(r->acc.userName,name)==0){
            check =1;
            break;
            }
        r =r->next;
    }
    if(check!=1) printf("Cannot find account!!!\n");
    else if(r->acc.status==0) printf("Account is blocked\n");
    else{
        do{
            printf("\nPassword: ");scanf("%s",pass);
            if(strcmp(pass,r->acc.password)!=0){
                if(check2==2){
                    r->acc.status=0;
                    writeFile(root);
                    printf("Password is incorrect. Account is blocked\n");
                    break;
                    } 
                ++check2;
                printf("Password incorrect!\n");
            }else{
                printf("Hello %s\n",name);
                r->statusLogin=1;
                break;
                }
        }while(check2<3);
        
    }
}

void search(node *root){
    char name[50];
    int check=0;
    node *r=root;
    printf("-------------Search------------\n");
    printf("Username: ");scanf("%s",name);
    while (r!=NULL){
        if(strcmp(r->acc.userName,name)==0){
            check =1;
            break;
        }
        r =r->next;
    }
    if(check!=1) printf("Cannot find account!!!\n");
    else if(r->acc.status==0) printf("Account is blocked\n");
    else printf("Account is active!!\n");
}

void logout(node *root){
    char name[50];
    int check=0;
    node *r=root;
    printf("-------------Logout------------\n");
    printf("Username: ");scanf("%s",name);
    while (r!=NULL){
        if(strcmp(r->acc.userName,name)==0){
            check =1;
            break;
        }
        r =r->next;
    }
    if(check!=1) printf("Cannot find account!!!\n");
    else if(r->statusLogin!=1) printf("Account is not login\n");
    else{
        printf("Goodbye %s!!!\n",r->acc.userName);
        r->statusLogin=0;
    }
}

void readFile(node **root)
{
    account temp;
    f=fopen("account.txt","r");
    if(f!=NULL)
    {
        while(fscanf(f,"%s %s %d",temp.userName, temp.password,&temp.status)!=EOF)
        {
            pushToList(root,temp);
        }
    }
    fclose(f);
}

void writeFile(node *root){
    f=fopen("account.txt","wt");
    while (root!=NULL)
    {
        fprintf(f,"%s %s %d\n", root->acc.userName,root->acc.password,root->acc.status);
        root = root->next;
    }
    fclose(f);   
}