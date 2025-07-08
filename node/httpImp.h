//
// Created by gogo on 6/25/25.
//

#ifndef HTTPIMP_H
#define HTTPIMP_H
#include "json-tools.h"
#define MAXDATASIZE 2097152 //2MB
char * handle_get(char *path);
char * handle_post(char * request, char* path);
char * procces_request(char * method, char * path,char * request);

//status code
char * handle_login();
char * handle_process(char * request);
char * handle_exit();

#endif //HTTPIMP_H
