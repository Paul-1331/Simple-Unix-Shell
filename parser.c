#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

void parse_input(char *input, command_t *cmd)
{
    memset(cmd, 0, sizeof(command_t));

    int i = 0, j = 0;
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        if (strcmp(token, "|") == 0)
        {
            cmd->is_pipe = 1;
            j = 0;
        }
        else if (strcmp(token, "&") == 0)
        {
            cmd->background = 1;
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " ");
            cmd->output_file = token;
        }
        else if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " ");
            cmd->input_file = token;
        }
        else
        {
            if (!cmd->is_pipe)
                cmd->args1[i++] = token;
            else
                cmd->args2[j++] = token;
        }
        token = strtok(NULL, " ");
    }
}