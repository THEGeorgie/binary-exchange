//
// Created by gogo on 6/27/25.
//
#include <json-c/json.h>
#include <stdio.h>

#ifndef JSON_TOOLS_H
#define JSON_TOOLS_H
#define MAXDATASIZE 2097152 //2MB
void create_js_paket_login_client(json_object *jobj, char *type, const char **out);
void create_js_paket_login_server(json_object *jobj, int id, const char **out);
void create_js_paket_process_client(json_object *jobj, char * program, int data[], const char **out);
void create_js_paket_process_server(json_object *jobj, const char **out);
void extract_js_packet(json_object *jobj, char key[], char **out);
void extract_js_packet_int(json_object *jobj, char key[], int *out);

#endif //JSON_TOOLS_H
