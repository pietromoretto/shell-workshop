#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.c"

void close_all_pipes(int n_pipes, int (*pipes)[2]) {
    for(int i = 0; i < n_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

int run(cmd_struct* command, int n_pipes, int (*pipes)[2]) {
    int pid = fork();
    if(pid < 0) {
        fprintf(stderr, "Fork failed\n");
    } else if(pid == 0) { // child process
        int fd = -1;
        if ((fd = command->redirect[0]) != -1) {
            dup2(fd, STDIN_FILENO);
        }
        if ((fd = command->redirect[1]) != -1) {
            dup2(fd, STDOUT_FILENO);
        }
        close_all_pipes(n_pipes, pipes);
        
        // handle cd special case
        if(strcmp(command->progname, "cd") == 0) {
            return chdir(command->args[1]);
        } else {
            return execvp(command->progname, command->args);
        }
    }
}

int main() {
    char *prompt = "DO$ "; // insert your prompt here
    char *line = NULL;
    size_t len = 0;
    
    while(fputs(prompt, stdout)) {
        getline(&line, &len, stdin);
        
        pipeline_struct *pipeline = parse_pipeline(line);
        int n_pipes = pipeline->n_cmds - 1;
        int (*pipes)[2] = calloc(sizeof(int[2]), n_pipes);
        
        // i start from 1 to ensure there is more than 1 command
        for(int i = 1; i < pipeline->n_cmds; i++) { 
            pipe(pipes[i-1]);
            pipeline->cmds[i-1]->redirect[STDOUT_FILENO] = pipes[i-1][1];
            pipeline->cmds[i]->redirect[STDIN_FILENO] = pipes[i-1][0];
        } 

        for(int i = 0; i < pipeline->n_cmds; i++) {
           run(pipeline->cmds[i], n_pipes, pipes); 
        } 
        
        close_all_pipes(n_pipes, pipes);

        for(int i = 0; i < pipeline->n_cmds; i++) {
            wait(NULL);
        }
    }
    return 0;
}
