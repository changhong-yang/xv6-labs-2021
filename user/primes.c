#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime_process(int read_channel)
{
    int pipeToLeft[2];
    int left = 0;
    int prime, number;
    read(read_channel, &prime, 4);
    printf("prime %d\n", prime);

    while (read(read_channel, &number, 4) != 0)
    {
        if (number % prime != 0)
        {
            // sent to the left
            if (left == 0)
            {
                pipe(pipeToLeft);
                if (fork() == 0)
                {
                    // read
                    close(pipeToLeft[1]);
                    prime_process(pipeToLeft[0]);
                }
                left = 1;
            }

            write(pipeToLeft[1], &number, 4);
        }
    }
    if (left != 0)
    {
        close(pipeToLeft[1]);
        wait((int *)0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int p[2]; // pipe  which sends numbers from 2 to 35
    pipe(p);
    if (fork() != 0)
    {
        // Parent Process
        close(p[0]);
        for (int i = 2; i <= 35; i++)
        {
            write(p[1], &i, 4);
        }
        close(p[1]);
        wait((int *)0);
        exit(0);
    }
    else
    {
        // Child Process
        close(p[1]);
        prime_process(p[0]);
    }
    exit(0);
}