#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.c"

int main() {
    char *prompt = "DO$ "; // insert your prompt here
    char *line = NULL;
    size_t len = 0;
    
    while(fputs(prompt, stdout)) {
        getline(&line, &len, stdin);
        cmd_struct *command = parse_command(line);
        execvp(command->progname, command->args);
    }
    return 0;
}
