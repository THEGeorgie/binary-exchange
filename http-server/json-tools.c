//
// Created by gogo on 6/27/25.
//
#include "json-tools.h"

#include <string.h>
//request
void create_js_paket_login_client(json_object *jobj, char *type, const char **out) {
    json_object_object_add(jobj, "deviceType", json_object_new_string(type));
    *out = strdup(json_object_to_json_string(jobj));
}

void create_js_paket_process_client(json_object *jobj, char *program, int data[], const char **out) {
    json_object_object_add(jobj, "program", json_object_new_string(program));
    json_object *dataArray = json_object_new_array_ext(sizeof(data)/sizeof(data[0]));

    for (int i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
        json_object_array_add(dataArray,json_object_new_int(data[i]));
    }

    json_object_object_add(jobj, "data", dataArray);

    *out = strdup(json_object_to_json_string(jobj));
}

//response
void create_js_paket_login_server(json_object *jobj, int id, const char **out) {
    json_object_object_add(jobj, "id", json_object_new_int(id));
    *out = strdup(json_object_to_json_string(jobj));
}

void create_js_paket_proccess_ticket_server(json_object *jobj, int pId, char *pName, const char **out) {
    json_object_object_add(jobj, "processId", json_object_new_int(pId));
    json_object_object_add(jobj, "processName", json_object_new_string(pName));

    *out = strdup(json_object_to_json_string(jobj));
}

void extract_js_packet(json_object *jobj, char* key, char **out) {
    if (!jobj) {
        printf("Failed to parse JSON\n");
        *out = NULL;
        return;
    }
    json_object *jobjC;
    if (json_object_object_get_ex(jobj, key, &jobjC)) {
        *out = strdup(json_object_get_string(jobjC));
    } else {
        printf("Key not found: %s\n", key);
        *out = NULL;
    }
}

void extract_js_packet_int(json_object *jobj, char key[], int *out) {
    if (!jobj) {
        printf("Failed to parse JSON\n");
        out = -1;
        return;
    }
    json_object *jobjC;
    if (json_object_object_get_ex(jobj, key, &jobjC)) {
        *out = strdup(json_object_get_int(jobjC));
    } else {
        printf("Key not found: %s\n", key);
        out = -1;
    }
}


