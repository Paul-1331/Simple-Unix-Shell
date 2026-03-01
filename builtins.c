#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "builtins.h"

int handle_builtin(command_t *cmd)
{
    if (!cmd->args1[0])
        return 1;

    if (strcmp(cmd->args1[0], "exit") == 0)
    {
        exit(0);
    }

    if (strcmp(cmd->args1[0], "cd") == 0)
    {
        if (cmd->args1[1])
            chdir(cmd->args1[1]);
        else
            fprintf(stderr, "cd: missing argument\n");
        return 1;
    }

    return 0;
}