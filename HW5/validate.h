/*
* validate user request
* feature: valid username, valid password, valid filename
*/


#ifndef __VALIDATE_H__
#define __VALIDATE_H__

#define MIN_LENGTH_OF_USERNAME 6
#define MAX_LENGTH_OF_USERNAME 30
#define MIN_LENGTH_OF_PASSWORD 3
#define MAX_LENGTH_OF_PASSWORD 20
//
int validateUsername(char* username);
//
int validatePassword(char* password);

int validNumber(char *value);

int checkDots(char *str);

int checkIP(char *str);

int validPortNumber(int port);

int hasIPAddress(char *ip);
#endif
