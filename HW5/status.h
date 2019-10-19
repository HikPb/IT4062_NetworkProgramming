/*
* define status 
* feature: 
*/

#ifndef __STATUS_H__
#define __STATUS_H__

typedef enum {
	USER_NOT_FOUND = 101,
	USER_IS_BLOCKED = 102,
	BLOCKED_USER = 103,
	PASSWORD_INVALID = 104,
	FILE_NOT_FOUND = 108,
	LOGIN_SUCCESS = 201,
	USER_IS_ONLINE = 105,
	ACCOUNT_IS_EXIST = 106,
	REGISTER_SUCCESS = 202,
	LOGOUT_SUCCESS = 203,
	USERNAME_OR_PASSWORD_INVALID = 107,
	COMMAND_INVALID = 301,
	SERVER_ERROR = 500
} StatusCode;

typedef enum{
	UNDER_SOCK,
	SOCK
} SocketType;

void messageCode(StatusCode code, char *msg);
#endif