//
// Created by gogo on 6/25/25.
//
#include "httpImp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * handle_get(char *path) {
    char *response = malloc(MAXDATASIZE);
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s","{\"id\": 1}");
    return response;
}

char * handle_post(char * request, char* path) {

    char *body = strstr(request, "\r\n\r\n");

    char *handler_result = NULL;
    char *response = NULL;

    if (strcmp(path, "/login") == 0) {
        handler_result = handle_login();
    }
    else if (strcmp(path, "/process") == 0) {
        handler_result = handle_process(body);
    }
    else if (strcmp(path, "/exit") == 0) {
        handler_result = handle_exit(body);
    }
    else {
        return strdup("HTTP/1.1 404 Not Found\r\n\r\n");
    }

    if (handler_result) {
        int needed_size = snprintf(NULL, 0, "POST /login HTTP/1.1\r\nHost: localhost:3490\r\nContent-Type: application/json\r\n\r\n%s", handler_result);
        response = malloc(needed_size + 1);
        if (response) {
            sprintf(response, "POST /login HTTP/1.1\r\nHost: localhost:3490\r\nContent-Type: application/json\r\n\r\n%s", handler_result);
        }
    }

    if (!response) {
        response = strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
    }
    return response;
}

char * handle_login() {

    json_object * login = json_object_new_object();
    char * request;
    create_js_paket_login_client(login, "N", &request);
    json_object_put(login);

    return request;
}

char * handle_process(char * request) {
    return "HTTP/1.1 404 Not Found \r\n\r\nBad request!!!\n";
}

char * procces_request(char * method, char * path,char * request) {
    if (strcmp(method, "GET") == 0) {
        return handle_get(path);
    }

    if (strcmp(method, "POST") == 0) {
        return handle_post(request, path);
    }

    return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n:(");
}

char * handle_exit() {
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n'{\"exit\": true}'";
}