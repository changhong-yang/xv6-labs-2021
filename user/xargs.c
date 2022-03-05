#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xorgs commands...\n");
        exit(1);
    }
    // set up commands and parameters
    char *command = argv[1];
    char *parameters[argc];
    parameters[0] = command;
    for (int i = 2; i < argc; i++)
    {
        parameters[i - 1] = argv[i];
    }

    // buffer stores inputs
    char buf[255];
    int index = 0;
    while (read(0, &buf[index], 1) != 0)
    {
        if (buf[index] == '\n')
        {
            buf[index] = 0;
            // proceed with command;
            parameters[argc - 1] = buf;
            if (fork() == 0)
            {
                exec(command, parameters);
                printf("Error\n");
            }
            else
            {
                wait((int *)0);
            }
            index = 0;
        }
        else
        {
            if (index == 254)
            {
                printf("buffer size exceed!\n");
                exit(1);
            }
            index += 1;
        }
    }
    exit(0);
}