#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int myint;
    int n = 5;
    int pid;
    for (int i = 0; i < n; i++) {
        pid = fork();
    }
    if (pid == 0) {
        int *ptr = NULL;
        printf("I'm the son %d.\n", getpid());
        return 1;
    } else {
        for (int i = 0; i < n; i++) {
            int sonStatus = wait(&myint);
            printf("Son Status: %d\n", myint);
        }
    }
    return 0;
}

// see Matt's code for this