#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    pid_t pid = fork();
    int myint;
    if (pid == 0) {
        printf("I'm %d, son of process %d\n", getpid(), getppid());
    } else {
        printf("I'm the father, %d. My son is: %d\n", getpid(), pid);
        wait(&myint);
    }
    return 0;
}
