#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    /*
     * A pair of pipe
     * One for parent sending message to child
     * One for child sending message back to parent
     */
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);

    int pid = fork();

    if (pid != 0)
    {
        // Parent Process
        char buffer[1];
        close(p1[0]);
        close(p2[1]);
        write(p1[1], "H", 1);
        read(p2[0], buffer, 1);
        close(p1[1]);
        close(p2[0]);
        int currentPid = getpid();
        printf("%d: received pong\n", currentPid);
        exit(0);
    }
    else
    {
        // Child Process
        char buffer[1];
        close(p1[1]);
        close(p2[0]);
        read(p1[0], buffer, 1);
        int currentPid = getpid();
        printf("%d: received ping\n", currentPid);
        write(p2[1], "H", 1);
        close(p1[1]);
        close(p2[0]);
        exit(0);
    }

    exit(0);
}