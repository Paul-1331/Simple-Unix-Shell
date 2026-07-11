#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "parser.h"
#include "executor.h"
#include "jobs.h"

#define MAX 1024

int main()
{
    signal(SIGINT, SIG_IGN);
    
    char input[MAX];
    command_t cmd;

    init_jobs();

    while (1)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%s@myshell:%s$ ", getenv("USER"), cwd);

        if (!fgets(input, MAX, stdin))
            break;

        input[strcspn(input, "\n")] = 0;

        parse_input(input, &cmd);

        if (!cmd.args1[0])
            continue;

        execute_command(&cmd);
    }

    return 0;
}