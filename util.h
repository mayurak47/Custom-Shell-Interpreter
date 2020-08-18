#include "global_variables.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 64
#define DELIMS " \t\n\r\a"
#define true 1

void read_line();
void split_line();
void shell_launch();
void cd();
void shell_exit();
void execute();
void command_loop();
