#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: sleep #number of ticks\n");
        exit(1);
    }

    int tickNumber = atoi(argv[1]);

    sleep(tickNumber);

    exit(0);
}