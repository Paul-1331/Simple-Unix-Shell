#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include "executor.h"
#include "builtins.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MIN3(a, b, c) MIN(a, MIN(b, c))

//calculates the minimum number of operations 
// (insertions, deletions, substitutions, and transpositions) required to change one word into another.
int damerau_levenshtein(const char *s1, const char *s2) {
    int s1len = strlen(s1);
    int s2len = strlen(s2);
    
    int dp[s1len + 1][s2len + 1];
    
    for (int i = 0; i <= s1len; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= s2len; j++) {
        dp[0][j] = j;
    }
    
    for (int i = 1; i <= s1len; i++) {
        for (int j = 1; j <= s2len; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1]; 
            } else {
                dp[i][j] = MIN3(dp[i - 1][j] + 1,      
                                dp[i][j - 1] + 1,      
                                dp[i - 1][j - 1] + 1); 
            }
            
            // Damerau-Levenshtein Transposition
            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1]) {
                dp[i][j] = MIN(dp[i][j], dp[i - 2][j - 2] + 1);
            }
        }
    }
    
    return dp[s1len][s2len];
}

void suggest_command(const char *cmd_name) {
    perror("exec failed");
    const char *common_cmds[] = {"ls", "cd", "pwd", "grep", "cat", "make", "gcc", "clear", "echo", "rm", "mkdir", "mv", "cp", "sleep"};
    int num_cmds = sizeof(common_cmds) / sizeof(char *);
    int min_dist = 100;
    const char *best_match = NULL;
    for (int i = 0; i < num_cmds; i++) {
        int dist = damerau_levenshtein(cmd_name, common_cmds[i]);
        if (dist < min_dist) {
            min_dist = dist;
            best_match = common_cmds[i];
        }
    }
    if (best_match && min_dist <= 2) {
        fprintf(stderr, "Command '%s' not found. Did you mean '%s'?\n", cmd_name, best_match);
    } else {
        fprintf(stderr, "Command '%s' not found.\n", cmd_name);
    }
}

void execute_single(command_t *cmd)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
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
        suggest_command(cmd->args1[0]);
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
        signal(SIGINT, SIG_DFL);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(cmd->args1[0], cmd->args1);
        suggest_command(cmd->args1[0]);
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        signal(SIGINT, SIG_DFL);
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        execvp(cmd->args2[0], cmd->args2);
        suggest_command(cmd->args2[0]);
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
    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (handle_builtin(cmd))
        return;

    if (cmd->is_pipe)
        execute_pipe(cmd);
    else
        execute_single(cmd);

    if (!cmd->background) {
        gettimeofday(&end, NULL);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        if (elapsed >= 0.0) {
            printf("[Executed in %.3fs]\n", elapsed);
        }
    }
}