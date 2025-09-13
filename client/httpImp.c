//
// Created by gogo on 6/25/25.
//
#include "httpImp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global-variables.h"

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


char * handle_get(char *path) {
    char *response = malloc(MAXDATASIZE);
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s","{\"id\": 1}");
    return response;
}

char * handle_post(char* path, char* data, char * program) {

    char *handler_result = NULL;
    char * response;

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

        if ( asprintf(&response, "POST %s HTTP/1.1\r\nHost: localhost:3490\r\nContent-Type: application/json\r\n\r\n%s",path, handler_result) == -1) {
            perror("asprintf");
        }

        free(handler_result);
        return response;
    }
    return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
}

char * handle_login() {

    json_object * login = json_object_new_object();
    char * request;
    create_js_paket_login_client(login, "C", &request);
    json_object_put(login);

    return request;
}

char * handle_process(char * data, char* program) {

    /*
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
    */

    FILE * fp = fopen(program, "rb");

    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);
    rewind(fp);

    unsigned char * programF = malloc(fileSize);
    if (programF == NULL) {
        perror("malloc");
    }
    size_t bytesRead = fread(programF, sizeof(*programF), fileSize, fp);
    if (bytesRead != fileSize) {
        fprintf(stderr, "Warning: only read %zu of %ld bytes\n", bytesRead, fileSize);
    }
    fclose(fp);

    fp = fopen(data, "r");
    char line[100];

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


    size_t base64_len;
    char * base64_encoded_program = base64_encode(programF, fileSize, &base64_len);


    char *request;

    json_object * Jprocess = json_object_new_object();
    create_js_paket_process_client(Jprocess,base64_encoded_program,jarray,&request);

    json_object_put(Jprocess);

    free(base64_encoded_program);
    free(programF);

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
    return strdup("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n'{\"exit\": true}'");
}

char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}




