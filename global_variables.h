#define BUFSIZE 64
#define DELIMS " \t\n\r\a"
#define true 1
#define false 0
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

char *line;
char **args;
char **history;
char pwd_buffer[256];
char user_buffer[256];
int return_status;
int curr_command;
int curr_history_size;

char *builtin_commands[] = {"cd", "exit", "history"};