#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int main() {
    char *line = NULL;
    size_t len = 0;
    
    while(fputs("DO> ", stdout)) {
        getline(&line, &len, stdin);
        printf("%s", line);
    }
    return 0;
}
