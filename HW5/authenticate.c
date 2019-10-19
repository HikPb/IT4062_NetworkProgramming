#include "authenticate.h"
#include "status.h"


User *head = NULL;
User *current = NULL;
// create new user from username, password, status
User *createNewUser(char* username, char* password, int status) {
	User *newUser = (User*)malloc(sizeof(User));
	strcpy(newUser->username, username);
	strcpy(newUser->password, password);
	newUser->status = status;
	newUser->isLogin = OFFLINE;
	newUser->countFails = 0;
	newUser->next = NULL;
	return newUser;
}

// print List users
void printList() {
    User *ptr = head;
    while (ptr != NULL) {
        printf("Username: %s\n", ptr->username);
        printf("Status: %s\n", ((ptr->status == ACTIVE) ? "Active" : "Blocked"));
        printf("isOnline: %s\n", ((ptr->isLogin == OFFLINE) ? "OFFLINE" : "ONLINE"));
        printf("----------------------------------\n");

        ptr = ptr->next;
    }
}

// add user to end of list
void append(User* newUser) {
	if(head == NULL) {
		head = newUser;
		current = newUser;
	}
 	else {
 		current->next = (User*)malloc(sizeof(User));
		current->next = newUser;
		current = newUser;
 	}
}

// ad user to beginning of list
void prepend(User** head, User * newUser) {
	User * new_node;
    new_node = (User*)malloc(sizeof(User));
    new_node = newUser;
    new_node->next = *head;
    *head = new_node;
    return;
}

// get List User
// readfile and create list users
void getListUser(char* fileName); 

// updateListUser
void updateListUser(char* fileName);

// fucntional
User* searchUser(char *username) {
	User *ptr = head;
    while (ptr != NULL) {
    	if(!strcmp(ptr->username, username)) break;
        ptr = ptr->next;
    }
    
	return ptr;
}


// compare password 
int identifyPassWord(User* user, char* password){
	return strcmp(user->password, password);
}

int isOnline(char *username){
	User *user = searchUser(username);
	return user->isLogin;
}
// login
int login(char* username, char* password){
	User *user = searchUser(username);
	if(user == NULL) return USER_NOT_FOUND;
	else{
		if(user->isLogin == ONLINE) return USER_IS_ONLINE;
		else if(user->status == ACTIVE){
			if(!identifyPassWord(user, password)){
				user->isLogin = ONLINE;
				user->countFails = 0;
				return LOGIN_SUCCESS;
			}
			else{
				user->countFails ++;
				if(user->countFails == MAX_LOGIN_FAILS){
					user->status = BLOCK;
					updateFile();
					return BLOCKED_USER;
				}
				return PASSWORD_INVALID;
			}
		}
		else 
			return USER_IS_BLOCKED;
	}
}
// exit program
void exitProgram() {
	printf("End Program : File %s not existed\n", ACCOUNT_FILE);
	exit(0);
}

void updateFile() {
	FILE* fOut;
	fOut = fopen(ACCOUNT_FILE, "w");
	if(!fOut) {
		printf("File not exist??\n");
		exitProgram();
	}

	User *ptr = head;

    while (ptr != NULL) {
        fprintf(fOut, "%s %s %d\n", ptr->username, ptr->password, ptr->status);    	
    	ptr = ptr->next;
    }

    fclose(fOut);
    return;
}
// read file account.txt
void readFile() {
	char username[254];
	char password[32];
	int status;
	char c;
	FILE* fIn;
	fIn = fopen(ACCOUNT_FILE, "r");
	if(!fIn) {
		printf("File not exist??\n");
		exitProgram();
	}

	while(!feof(fIn)) {
		if(fscanf(fIn, "%s %s %d%c", username, password, &status, &c) != EOF) {
			append(createNewUser(username, password, status));
		}
		if(feof(fIn)) break;
	}
	fclose(fIn);
	return;
}

// register
int registerUser(char* username, char* password){
	User *user = searchUser(username);
	if(user == NULL){
		user = createNewUser(username, password, ACTIVE);
		user->isLogin = ONLINE;
		append(user);
		updateFile();
		return REGISTER_SUCCESS;
	}
	return ACCOUNT_IS_EXIST;
}

int logoutUser(char *username){
	User *user = searchUser(username);
	if(user->isLogin == ONLINE) {
		user->isLogin = OFFLINE;
		return LOGOUT_SUCCESS;
	} else {
		return COMMAND_INVALID;
	}
	return SERVER_ERROR;
}

