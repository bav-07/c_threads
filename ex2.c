#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int myint;
    int pid = fork();
    if (pid == 0) {
        int *ptr = NULL;
        printf("I'm the son %d.\n", getpid());
        int myint = *ptr;
        return 1;
    } else {
        int sonStatus = wait(&myint);
        printf("Son Status: %d\n", myint);
    }
    return 0;
}
