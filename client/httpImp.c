//
// Created by gogo on 6/25/25.
//
#include "httpImp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global-variables.h"

char * handle_get(char *path) {
    char *response = malloc(MAXDATASIZE);
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s","{\"id\": 1}");
    return response;
}

char * handle_post(char* path, char* data, char * program) {

    char *handler_result = NULL;
    char * response = NULL;

    if (strcmp(path, "/login") == 0) {
        handler_result = handle_login();
    }
    else if (strcmp(path, "/process") == 0) {
        handler_result = handle_process(data,program);
    }
    else if (strcmp(path, "/exit") == 0) {
        handler_result = handle_exit();
    }
    else {
        return strdup("HTTP/1.1 404 Not Found\r\n\r\n");
    }

    if (handler_result) {
        size_t needed_size = snprintf(NULL, 0, "POST /login HTTP/1.1\r\nHost: localhost:3490\r\nContent-Type: application/json\r\n\r\n%s", handler_result);
        response = malloc(needed_size+1);
        if (response == NULL) {
            perror("malloc");
        }
        if (response) {
            sprintf(response, "POST /login HTTP/1.1\r\nHost: localhost:3490\r\nContent-Type: application/json\r\n\r\n%s", handler_result);
        }
        if (!response) {
            response = strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        }
        free(handler_result);
        return response;
    }

}

char * handle_login() {

    json_object * login = json_object_new_object();
    char * request;
    create_js_paket_login_client(login, "C", &request);
    json_object_put(login);

    return request;
}

char * handle_process(char * data, char* program) {
    FILE * fp = fopen(data, "r");
    char line[MAXDATASIZE];

    json_object *jarray = json_object_new_array();

    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ",");
        while (token) {
            double val = atof(token);
            json_object_array_add(jarray, json_object_new_double(val));
            token = strtok(NULL, ",");
        }
    }
    fclose(fp);

    fp = fopen(program, "rb");

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    unsigned char * programF = malloc(fileSize);
    if (programF == NULL) {
        perror("malloc");
    }
    size_t bytesRead = fread(programF, 1, fileSize, fp);
    if (bytesRead != fileSize) {
        fprintf(stderr, "Warning: only read %zu of %ld bytes\n", bytesRead, fileSize);
    }
    fclose(fp);

    size_t base64_len;
    char *base64_str_program = base64_encode(programF, fileSize, &base64_len);
    char *request;

    json_object * Jprocess = json_object_new_object();
    create_js_paket_process_client(Jprocess,base64_str_program,jarray,&request);

    json_object_put(Jprocess);
    json_object_put(jarray);

    free(programF);
    free(base64_str_program);
    return request;
}

char * create_request(char * method, char * path, char* data, char * program) {
    if (strcmp(method, "GET") == 0) {
        return handle_get(path);
    }

    if (strcmp(method, "POST") == 0) {
        return handle_post(path, data, program);
    }

    return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n:(");
}

char * handle_exit() {
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n'{\"exit\": true}'";
}

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *encoded = malloc((4 * ((input_length + 2) / 3)) + 1);
    if (!encoded) return NULL;

    *output_length = 0;
    for (size_t i = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded[(*output_length)++] = encoding_table[(triple >> 18) & 0x3F];
        encoded[(*output_length)++] = encoding_table[(triple >> 12) & 0x3F];
        encoded[(*output_length)++] = (i > input_length + 1) ? '=' : encoding_table[(triple >> 6) & 0x3F];
        encoded[(*output_length)++] = (i > input_length)     ? '=' : encoding_table[triple & 0x3F];
    }

    encoded[*output_length] = '\0';
    return encoded;
}
