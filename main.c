#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_BUFFER 4096

typedef char *string;

typedef struct
{
    char buffer[MAX_BUFFER]; // temp storage for current word
    int buffer_index; // index of last data in buffer

    string *tokens; // array of tokenized commands
    int token_count; // number of tokens
    int token_cap; // max number of tokens allowed
} Tokenizer;

Tokenizer *init_tokenizer()
{
    Tokenizer *t = malloc(sizeof(Tokenizer));
    t->buffer_index = 0;
    t->token_count = 0;
    t->token_cap = 10;
    t->tokens = malloc(sizeof(string) * t->token_cap);
    t->buffer[0] = '\0';
    return t;
}

void append_char_to_buffer(Tokenizer *t, char c)
{
    if (t->buffer_index < MAX_BUFFER - 1)
    {
        t->buffer[t->buffer_index++] = c;
    }
    else
    {
        printf("BUFFER IS FULL\n");
    }
}

void save_token(Tokenizer *t)
{
    if (t->buffer_index == 0) return;

    t->buffer[t->buffer_index] = '\0';

    if (t->token_count >= t->token_cap)
    {
        t->token_cap *= 2;
        t->tokens = realloc(t->tokens, sizeof(string) * t->token_cap);
    }

    string temp = malloc(sizeof(char) * (t->buffer_index + 1));
    if (temp == NULL)
    {
        return;
    }
    for (int i = 0; i <= t->buffer_index; i++)
    {
        temp[i] = t->buffer[i];
    }
    t->tokens[t->token_count] = temp;
    t->token_count++;
    t->buffer_index = 0;
}

void free_tokens(string *tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

string *tokenize_input(const string input)
{
    Tokenizer *t = init_tokenizer();

    int in_quotes = false;

    for (int i = 0; input[i] != '\0'; i++)
    {
        char c = input[i];

        if (c == '"' || c == '\'')
        {
            in_quotes = !in_quotes;
            continue;
        }

        if (c == ' ' && !in_quotes)
        {
            save_token(t);
            continue;
        }

        append_char_to_buffer(t, c);

    }

    save_token(t);
    t->tokens[t->token_count++] = NULL;
    string *results = t->tokens;
    free(t);
    return results;
}

string read_input()
{
    int capacity = 16;
    int length = 0;
    string buffer = malloc(capacity * sizeof(char));

    if (buffer == NULL)
        return NULL;

    int ch;

    while ((ch = fgetc(stdin)) != '\n' && ch != EOF)
    {
        if (length + 1 >= capacity)
        {
            capacity *= 2;
            string temp = realloc(buffer, capacity * sizeof(char));
            if (temp == NULL)
            {
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }
        buffer[length++] = (char)ch;
    }
    buffer[length] = '\0';

    string final_buffer = realloc(buffer, (length + 1) * sizeof(char));
    return final_buffer ? final_buffer : buffer;
}

int main(void)
{
    while (1)
    {
        printf("myshell> ");
        string input = read_input();

        if (input == NULL || *input == '\0')
            break;

        string *tokens = tokenize_input(input);
        for (int i = 0; tokens[i] != NULL; i++)
        {
            printf("%s\n", tokens[i]);
        }

        free_tokens(tokens);
        free(input);
    }
}
