#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <history.h>
#include <limits.h>

#define MAX_BUFFER 4096

typedef char *string;

Node *current_node = NULL;

void change_dir(const string);

typedef struct
{
    char buffer[MAX_BUFFER]; // temp storage for current word
    int buffer_index;        // index of last data in buffer

    string *tokens;  // array of tokenized commands
    int token_count; // number of tokens
    int token_cap;   // max number of tokens allowed
} Tokenizer;

struct termios orig_termios;
volatile sig_atomic_t inRawMode = 0;

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

void disableRawMode()
{
    if (inRawMode)
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        inRawMode = 0;
    }
}

void handle_signal(int sig)
{
    disableRawMode();
    printf("\n");
    exit(0);
}

void enableRawMode()
{
    if (inRawMode) return;

    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    inRawMode = 1;
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
    if (t->buffer_index == 0)
        return;

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

    bool in_quotes = false;

    for (int i = 0; input[i] != '\0'; i++)
    {
        char c = input[i];

        if (c == '\\')
        {
            append_char_to_buffer(t, input[i + 1]);
            i++;
            continue;
        }

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

bool string_compare(const char *string_1, const char *string_2)
{
    int i = 0;
    while (string_1[i] != '\0')
    {
        if (string_1[i] != string_2[i])
        {
            return false;
        }
        i++;
    }

    return string_2[i] == '\0';
}

string string_concatenation(const string string_1, const string string_2)
{
    string concatenated_string = malloc(sizeof(char) * 100);
    int index = 0;
    for (int i = 0; string_1[i] != '\0'; i++)
    {
        concatenated_string[index++] = string_1[i];
    }
    concatenated_string[index++] = ' ';
    for (int i = 0; string_2[i] != '\0'; i++)
    {
        concatenated_string[index++] = string_2[i];
    }
    concatenated_string[index] = '\0';
    return concatenated_string;
}

string read_input()
{
    int capacity = 16;
    int length = 0;
    string buffer = malloc(capacity * sizeof(char));

    if (buffer == NULL)
        return NULL;

    fflush(stdout);
    enableRawMode();

    char ch;

    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n')
    {
        if (ch == '\033')
        {
            char seq[3];

            if (read(STDIN_FILENO, &seq[0], 1) == 0)
                continue;
            if (read(STDIN_FILENO, &seq[1], 1) == 0)
                continue;

            if (seq[0] == '[')
            {
                switch (seq[1])
                {
                case 'A':
                    if (current_node != NULL)
                    {
                        string cmd = current_node->command;
                        int cmd_len = string_length(cmd);

                        // Clear current buffer on screen
                        for (int i = 0; i < length; i++)
                        {
                            write(STDOUT_FILENO, "\b \b", 3);
                        }

                        // Copy command to buffer and display
                        length = 0;
                        for (int i = 0; i < cmd_len; i++)
                        {
                            buffer[length++] = cmd[i];
                            write(STDOUT_FILENO, &cmd[i], 1);
                        }

                        if (current_node->next != NULL)
                        {
                            current_node = current_node->next;
                        }
                        else
                        {
                            write(STDOUT_FILENO, "\a", 1);
                        }
                    }
                    continue;
                case 'B':
                    //printf("Down Arrow Pressed\r\n");
                    //printf("%i", current_node == NULL);
                    // write(STDOUT_FILENO, (current_node == NULL) ? "T" : "F", 1);
                    if (current_node != NULL && current_node->previous != NULL)
                    {
                        string current_cmd = current_node->command;
                        current_node = current_node->previous;
                        string cmd = current_node->command;
                        int cmd_len = string_length(cmd);

                        for (int i = 0; i <= string_length(current_cmd); i++)
                        {
                            write(STDOUT_FILENO, "\b \b", 3);
                        }

                        length = 0;
                        for (int i = 0; i < cmd_len; i++)
                        {
                            buffer[length++] = cmd[i];
                            write(STDOUT_FILENO, &cmd[i], 1);
                        }
                    }

                    continue;
                }
            }
            continue;
        }

        if (ch == 127) // Handle Backspace
        {
            if (length > 0)
            {
                length--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
            continue;
        }

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
        buffer[length++] = ch;
        write(STDOUT_FILENO, &ch, 1);
    }
    write(STDOUT_FILENO, "\n", 1);
    buffer[length] = '\0';
    disableRawMode();
    string final_buffer = realloc(buffer, (length + 1) * sizeof(char));
    return final_buffer ? final_buffer : buffer;
}

int exec_commands(const string *tokens)
{

    if (string_compare(tokens[0], "cd"))
    {
        change_dir(tokens[1]);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Error occurred while trying to create a child process\n");
        return 1;
    }
    else if (pid == 0)
    {
        // printf("the child's id process is %d\n", getpid());
        execvp(tokens[0], tokens);
    }
    else
    {
        // printf("the parent's process id is %d\n", getpid());
        wait(NULL);
        // printf("the child died. parent moving on.\n");
    }
    return 0;
}

void change_dir(const string directory_path)
{
    chdir(directory_path);
    return;
}

int main(void)
{
    signal(SIGINT, handle_signal);

    Node *head = NULL;
    current_node = head;

    char cwd[PATH_MAX];

    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("\033[1;34m%s\033[0m> ", cwd);
        }
        string input = read_input();

        if (string_length(input) > 0)
        {
            insert_element_at_head(&head, input);
        }

        if (input == NULL || *input == '\0')
            break;

        string *tokens = tokenize_input(input);
        // for (int i = 0; tokens[i] != NULL; i++)
        //{
        //     printf("%s\n", tokens[i]);
        // }
        exec_commands(tokens);
        free_tokens(tokens);
        free(input);
    }
}
