#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef char *string;

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

string *tokenizer(string line)
{
    string *tokens = malloc(5 * sizeof(string));

    int token_count = 0;

    string p = line;
    bool in_quotes = false;

    while (*p != "\0")
    {
        while (*p == " " && in_quotes) p++;
    }

}

int main(void)
{
    while (1)
    {
        printf("myshell> ");
        string input = read_input();

        if (input == NULL)
            break;

        printf("%s\n", input);
        free(input);
    }
}
