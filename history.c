#include <stdio.h>
#include <stdlib.h>

typedef char *string;

int string_length(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

void string_copy(char *dest, const char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

typedef struct Node
{
    string command;
    struct Node *previous;
    struct Node *next;
} Node;

Node *current_node = NULL;

void insert_element_at_head(Node **head, const string command)
{
    Node *new_node = malloc(sizeof(Node));
    int len = string_length(command);
    new_node->command = malloc(len + 1);
    string_copy(new_node->command, command);

    if (*head == NULL)
    {
        new_node->next = NULL;
        new_node->previous = NULL;
        *head = new_node;
        current_node = *head;
        return;
    }
    new_node->next = *head;
    (*head)->previous = new_node;
    new_node->previous = NULL;
    *head = new_node;
    current_node = *head;
}

string go_to_older_command()
{
    if (current_node == NULL)
    {
        return NULL;
    }
    if (current_node->next != NULL)
    {
        current_node = current_node->next;
        return current_node->command;
    }
    printf("\a\n");
    return NULL;
}

string go_to_newer_command()
{
    if (current_node == NULL)
    {
        return NULL;
    }
    if (current_node->previous != NULL)
    {
        current_node = current_node->previous;
        return current_node->command;
    }
    else
    {
        return NULL;
    }
}

void free_linked_list(Node *head)
{
    Node *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp->command);
        free(temp);
    }
}

int main()
{
    Node *head = NULL;
    current_node = head;
    insert_element_at_head(&head, "Hello world");
    insert_element_at_head(&head, "UNGA BUNGA");
    printf("%s\n", current_node->command);
    go_to_older_command();
    printf("%s\n", current_node->command);
    go_to_older_command();
    go_to_newer_command();
    printf("%s\n", current_node->command);

    free_linked_list(head);
    return 0;
}
