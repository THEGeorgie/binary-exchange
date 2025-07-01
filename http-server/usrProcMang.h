//
// Created by gogo on 6/27/25.
//

#ifndef USRPROCMANG_H
#define USRPROCMANG_H
#include <stdio.h>
#include <stdlib.h>

struct  Node {
    int data;
    struct Node *next;
};
extern struct Node * client_ids;
extern struct Node * node_ids;
void append(struct Node **head_ref, int new_data);
void free_list(struct Node **head);
int get_node_data(struct Node *head, int index);
int create_user(struct Node **head);
int get_list_length(struct Node *head);




#endif //USRPROCMANG_H
