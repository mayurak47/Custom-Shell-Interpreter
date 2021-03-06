#include "global_variables.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wordexp.h>

void read_line();
void split_line();
void expand_args();
void shell_launch();
void cd();
void shell_exit();
void execute();
void command_loop();
void shell_history();
void cleanup();

void (*builtin_func[]) () = {&cd, &shell_exit, &shell_history};

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

    int orig_line_len = strlen(line);
    char *line_copy = malloc((orig_line_len+1) * sizeof(char));
    strcpy(line_copy, line);

    curr_token = strtok(line_copy, DELIMS);
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

void expand_args()
{
    int buffer_size = BUFSIZE;
    int curr_arg_index = 0;
    int curr_exp_arg_index = 0;

    expanded_args = malloc(buffer_size * sizeof(char *));
    if(expanded_args == NULL)
    {
        fprintf(stderr, "Memory allocation error, exiting.\n");
        exit(EXIT_FAILURE);
    }

    wordexp_t p;
    char **w;
    
    while(args[curr_arg_index] != NULL)
    {
        wordexp(args[curr_arg_index], &p, 0);
        w = p.we_wordv;
        for(int i=0; i<p.we_wordc; i++)
        {
            expanded_args[curr_exp_arg_index] = malloc((strlen(w[i])+1) * sizeof(char));
            strcpy(expanded_args[curr_exp_arg_index], w[i]);
            curr_exp_arg_index++;
            if(curr_exp_arg_index >= buffer_size)
            {
                buffer_size *= 2;
                expanded_args = realloc(expanded_args, buffer_size);
                if(expanded_args == NULL)
                {
                    fprintf(stderr, "Memory allocation error, exiting.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        curr_arg_index++;
    }
    expanded_args[curr_exp_arg_index] = NULL;
    wordfree(&p);
    args = expanded_args;

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
            perror("exec error");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0)
    {
        perror("fork error");
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
            perror("chdir error");
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

void add_history()
{
    if(history == NULL)
    {
        history = malloc(curr_history_size * sizeof(char *));
        if(history == NULL)
        {
            fprintf(stderr, "Memory allocation error, exiting.\n");
            exit(EXIT_FAILURE);
        }
    }
    history[curr_command] = malloc((strlen(line)+1) * sizeof(char));
    strcpy (history[curr_command], line);
    curr_command++;
    if(curr_command>=curr_history_size)
    {
        curr_history_size *= 2;
        history = realloc(history, curr_history_size);
        if(history == NULL)
        {
            fprintf(stderr, "Memory allocation error, exiting.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void shell_history()
{
    int start_command = 0;
    if(args[1] != NULL)
    {
        start_command = MAX(curr_command - atoi(args[1]), 0);
    }
    for(int i=start_command; i<curr_command; i++)
    {
        printf("%d.\t%s\n", i+1, history[i]);
    }
}

void command_loop()
{
    history = NULL;
    return_status = 1;
    curr_command = 0;
    curr_history_size = BUFSIZE;
    while(true)
    {
        line = NULL;
        args = NULL;
        getcwd(pwd_buffer, sizeof(pwd_buffer));
        getlogin_r(user_buffer, sizeof(user_buffer));
        printf("%s@%s $ ", user_buffer, pwd_buffer);
        read_line();
        add_history();
        split_line();
        expand_args();
        execute();
        if(return_status == 0)
            break;
    }
}

void cleanup()
{
    free(line);
    free(args);
    for(int i=0; i<curr_command; i++)
    {
        free(history[i]);
    }
    free(history);
}