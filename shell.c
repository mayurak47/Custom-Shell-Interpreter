#include "util.c"

int main(int argc, char *argv[])
{
    command_loop();
    cleanup();
    return EXIT_SUCCESS;
}