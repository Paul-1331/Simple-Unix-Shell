#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64

typedef struct {
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];
    int is_pipe;
    int background;
    char *input_file;
    char *output_file;
} command_t;

void parse_input(char *input, command_t *cmd);

#endif