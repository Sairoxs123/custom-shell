#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>
#include <stdlib.h>

typedef char *string;

typedef struct Node
{
    string command;
    struct Node *previous;
    struct Node *next;
} Node;

int string_length(const string str);
void insert_element_at_head(Node **head, const string command);
string go_to_older_command();
string go_to_newer_command();
void string_copy(string dest, string src);
void free_linked_list(Node *head);

#endif