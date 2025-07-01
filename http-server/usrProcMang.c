//
// Created by gogo on 6/27/25.
//

#include "usrProcMang.h"
struct Node *client_ids = NULL;
struct Node *node_ids = NULL;

void append(struct Node **head_ref, int new_data) {
    struct Node *new_node = malloc(sizeof(struct Node));
    new_node->data = new_data;
    new_node->next = NULL;

    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }

    struct Node *last = *head_ref;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = new_node;
}

void free_list(struct Node **head) {
    struct Node *temp;
    while (*head != NULL) {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

int get_node_data(struct Node *head, int index) {
    int count = 0;
    while (head != NULL) {
        if (count == index) {
            return head->data;
        }
        count++;
        head = head->next;
    }

    printf("Error out of range");
    return -1;
}

int get_list_length(struct Node *head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }

    return count;
}

int create_user(struct Node **head) {
    int id = rand()% 1000;
    int status = 0;
    while (status == 0) {
        for (int i = 0; i < get_list_length(*head); i++) {
            if (get_node_data(*head, i) == id) {
                id = rand()% 1000;
                status = 0;
            }
        }
        status = 1;
    }

    append(head,id);
    return id;
}