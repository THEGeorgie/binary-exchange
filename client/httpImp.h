//
// Created by gogo on 6/25/25.
//

#ifndef HTTPIMP_H
#define HTTPIMP_H
#include "json-tools.h"
#define MAXDATASIZE 2097152 //2MB
char * handle_get(char *path);
char * handle_post(char* path, char* data, char * program);
char * create_request(char * method, char * path, char* data, char * program);
char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);

//status code
char * handle_login();
char * handle_process(char * data, char* program);
char * handle_exit();

#endif //HTTPIMP_H
