// create new user from username, password, status
RequestFile *createObjectFile(char* fileName, int requestId) {
	RequestFile *file = (RequestFile*)malloc(sizeof(RequestFile));
	strcpy(file->fileName, fileName);
	file->requestId = requestId;
	file->next = NULL;
	return file;
}

// add user to end of list
void append(RequestFile* newFile) {
	if(head == NULL) {
		head = newFile;
		current = newFile;
	}
 	else {
 		current->next = (RequestFile*)malloc(sizeof(RequestFile));
		current->next = newFile;
		current = newFile;
 	}
	return;
}

// ad user to beginning of list
void prepend(RequestFile** head, RequestFile * newFile) {
	RequestFile * new_node;
    new_node = (RequestFile*)malloc(sizeof(RequestFile));
    new_node = newFile;
    new_node->next = *head;
    *head = new_node;
    return;
}

// fucntional
RequestFile* search(int requestId) {
	RequestFile *ptr = head;

    while (ptr != NULL) {
    	if(ptr->requestId == requestId) break;
        ptr = ptr->next;
    }
    
	return ptr;
}

//
int remove(int requestId) {
	RequestFile *removeFile = search(requestId);
	RequestFile *ptr = head;

    while (ptr != NULL) {
    	if(ptr->next == removeFile) break;
        ptr = ptr->next;
    }
    ptr->next = ptr->next->next;
    free(removeFile);
}

int isFileExist(char *fileName) {
	FILE *fptr = fopen(fileName, 'r');
	if(fptr == NULL) {
		return 0;
	}
	return 1;
}

