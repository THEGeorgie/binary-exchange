//
// Created by gogo on 6/25/25.
//

#ifndef HTTPIMP_H
#define HTTPIMP_H
#include "json-tools.h"
#include  "usrProcMang.h"
#define MAXDATASIZE 2097152 //2MB
char * handle_get(char *path);
char * handle_post(char * request, char* path);
char * procces_request(char * request);

//status code
char * handle_login(char * request);
char * handle_process(char * request);

#endif //HTTPIMP_H
