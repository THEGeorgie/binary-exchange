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
        handler_result = handle_login(body);
    }
    else if (strcmp(path, "/process") == 0) {
        handler_result = handle_process(body);
    }
    else {
        return strdup("HTTP/1.1 404 Not Found\r\n\r\n");
    }

    if (handler_result) {
        int needed_size = snprintf(NULL, 0, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s", handler_result);
        response = malloc(needed_size + 1);
        if (response) {
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s", handler_result);
        }
    }

    if (!response) {
        response = strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
    }

    return response;
}

char * handle_login(char * request) {
    json_object * jdata = json_tokener_parse(request);
    char * out;
    extract_js_packet(jdata, "deviceType", &out);

    if (strcmp(out, "C") == 0) {
        int id = create_user(&client_ids);
        json_object * dataOutput = json_object_new_object();
        create_js_paket_login_server(dataOutput,id, &out);
        json_object_put(jdata);
        json_object_put(dataOutput);
        return out;
    }
    if (strcmp(out, "N") == 0) {
        int id = create_user(&node_ids);
        json_object * dataOutput = json_object_new_object();
        create_js_paket_login_server(dataOutput,id, &out);
        json_object_put(jdata);
        json_object_put(dataOutput);
        return out;
    }
    return "HTTP/1.1 404 Not Found \r\n\r\nBad request!!!\n";
}

char * handle_process(char * request) {
    return "HTTP/1.1 404 Not Found \r\n\r\nBad request!!!\n";
}

char * procces_request(char * request) {
    char method[10], path[50];
    sscanf(request, "%s %s", method, path);
    if (strcmp(method, "GET") == 0) {
        return handle_get(path);
    }

    if (strcmp(method, "POST") == 0) {
        return handle_post(request, path);
    }
    return "HTTP/1.1 404 Not Found \r\n\r\nBad request!!!\n";
}