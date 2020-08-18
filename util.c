#include "global_variables.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void read_line();
void split_line();
void shell_launch();
void cd();
void shell_exit();
void execute();
void command_loop();

void (*builtin_func[]) () = {&cd, &shell_exit};

#define num_builtins sizeof(builtin_commands)/sizeof(char *)

void read_line()
{
    int buffer_size = BUFSIZE;
    int curr_pos = 0;
    line = malloc(sizeof(char) * buffer_size);
    if(line == NULL)
    {
        fprintf(stderr, "Memory allocation error, exiting.\n");
        exit(EXIT_FAILURE);
    }
    while(true)
    {
        int ch = getchar();
        if(ch == EOF || ch == '\n')
        {
            line[curr_pos] = '\0';
            return;
        }
        else
        {
            line[curr_pos] = ch;
        }
        curr_pos++;
        if (curr_pos >= buffer_size)
        {
            buffer_size *= 2;
            line = realloc(line, buffer_size);
            if(line == NULL)
            {
                fprintf(stderr, "Memory allocation error, exiting.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void split_line()
{
    int buffer_size = BUFSIZE;
    int curr_pos = 0;
    args = malloc(buffer_size * sizeof(char *));
    char *curr_token;
    if(args == NULL)
    {
        fprintf(stderr, "Memory allocation error, exiting.\n");
        exit(EXIT_FAILURE);
    }

    curr_token = strtok(line, DELIMS);
    while(curr_token != NULL)
    {
        args[curr_pos] = curr_token;
        curr_pos++;

        if(curr_pos >= buffer_size)
        {
            buffer_size *= 2;
            args = realloc(args, buffer_size);
            if(args == NULL)
            {
                fprintf(stderr, "Memory allocation error, exiting.\n");
                exit(EXIT_FAILURE);
            }
        }

        curr_token = strtok(NULL, DELIMS);
    }
    args[curr_pos] = NULL;
}

void shell_launch()
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0)
    {
        if(execvp(args[0], args) == -1)
        {
            perror("exec error: ");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0)
    {
        perror("fork error: ");
    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}


void cd()
{
    if(args[1] == NULL)
    {
        fprintf(stderr, "Enter argument.\n");
    }
    else
    {
        if(chdir(args[1]) != 0)
        {
            perror("chdir error: ");
        }
    }
}

void shell_exit()
{
    return_status = 0;
}

void execute()
{
    if(args[0] == NULL)
    { 
        return;
    }

    for(int i=0; i<num_builtins; i++)
    {
        if(strcmp(args[0], builtin_commands[i]) == 0)
        {
            (*builtin_func[i])();
            return;
        }
    }

    shell_launch();
}



void command_loop()
{
    return_status = 1;
    while(true)
    {
        line = NULL;
        args = NULL;
        getcwd(pwd_buffer, sizeof(pwd_buffer));
        getlogin_r(user_buffer, sizeof(user_buffer));
        printf("%s@%s $ ", user_buffer, pwd_buffer);
        read_line();
        split_line();
        execute();
        if(return_status == 0)
            break;
    }
}

