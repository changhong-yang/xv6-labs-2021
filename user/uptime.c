#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{

    int number_of_tick = uptime();
    printf("%d clock ticks since start!\n", number_of_tick);

    exit(0);
}