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
