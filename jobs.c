#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>

void sigchld_handler(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void init_jobs()
{
    signal(SIGCHLD, sigchld_handler);
}