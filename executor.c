#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "executor.h"
#include "builtins.h"

void execute_single(command_t *cmd)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        if (cmd->input_file)
        {
            int fd = open(cmd->input_file, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (cmd->output_file)
        {
            int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execvp(cmd->args1[0], cmd->args1);
        perror("exec failed");
        exit(1);
    }
    else
    {
        if (!cmd->background)
            wait(NULL);
    }
}

void execute_pipe(command_t *cmd)
{
    int fd[2];
    pipe(fd);

    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(cmd->args1[0], cmd->args1);
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        execvp(cmd->args2[0], cmd->args2);
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    if (!cmd->background)
    {
        wait(NULL);
        wait(NULL);
    }
}

void execute_command(command_t *cmd)
{
    if (handle_builtin(cmd))
        return;

    if (cmd->is_pipe)
        execute_pipe(cmd);
    else
        execute_single(cmd);
}