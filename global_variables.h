#define BUFSIZE 64
#define DELIMS " \t\n\r\a"
#define true 1
#define false 0

char *line;
char **args;
char pwd_buffer[256];
char user_buffer[256];
int return_status;


char *builtin_commands[] = {"cd", "exit"};