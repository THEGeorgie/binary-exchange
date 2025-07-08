//
// Created by gogo on 6/27/25.
//
#include "json-tools.h"

#include <string.h>


//request
void create_js_paket_login_client(json_object *jobj, char *type,  char **out) {
    json_object_object_add(jobj, "deviceType", json_object_new_string(type));
    *out = strdup(json_object_to_json_string(jobj));
}

void create_js_paket_process_client(json_object *jobj, char * program, json_object * data,  char **out) {
    json_object_object_add(jobj, "deviceType", json_object_new_string("C"));
    json_object_object_add(jobj, "id", json_object_new_int(id));
    json_object_object_add(jobj, "program", json_object_new_string(program));
    json_object_object_add(jobj, "data", data);

    *out = strdup(json_object_to_json_string(jobj));
}

//response
void create_js_paket_login_server(json_object *jobj, int id,  char **out) {
    json_object_object_add(jobj, "id", json_object_new_int(id));
    *out = strdup(json_object_to_json_string(jobj));
}

void create_js_paket_proccess_ticket_server(json_object *jobj, int pId, char *pName,  char **out) {
    json_object_object_add(jobj, "processId", json_object_new_int(pId));
    json_object_object_add(jobj, "processName", json_object_new_string(pName));

    *out = strdup(json_object_to_json_string(jobj));
}

int extract_js_packet(json_object *jobj, char* key, char **out) {
    if (!jobj) {
        printf("Failed to parse JSON\n");
        return -1;
    }
    json_object *jobjC;
    if (json_object_object_get_ex(jobj, key, &jobjC)) {
        *out = strdup(json_object_get_string(jobjC));
    } else {
        printf("Key not found: %s\n", key);
        return -1;
    }
}

int extract_js_packet_int(json_object *jobj, char key[], int *out) {
    if (!jobj) {
        printf("Failed to parse JSON\n");
        return -1;
    }
    json_object *jobjC;
    if (json_object_object_get_ex(jobj, key, &jobjC)) {
        *out = json_object_get_int(jobjC);
    } else {
        printf("Key not found: %s\n", key);
        return -1;
    }
}


