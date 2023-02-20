#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define MAX_PATH 256
#define DEBUG 1
#define GREEN "\x1B[32m"
#define NORMAL "\x1B[0m"

char* makePrompt() {
    // get current working directory
    char cwd[MAX_PATH];
    getcwd(cwd, sizeof(cwd));

    // allocate memory for string promppt with two extra places
    //  one space is for the $ and the other is for the null terminator \0
    char* prompt = malloc(sizeof(char) * ((strlen(cwd) + 2)));
    strcpy(prompt, cwd);
    strcat(prompt, "$");

    return prompt;
}

void DebugPrint(char* message, char* data) {
    /*
    Only for Debugging purposes. Allows for some formatted text
    output. Makes distinction of (DEBUG) prints from normal output
    a bit clearer on the eyes.
    */
    printf("%s(DEBUG)%s %s \"%s\"\n", GREEN, NORMAL, message, data);
}

int handleInput(char* input) {
    /* Function takes an input and returns a 1 or 0.
    0 will cause the program to end. 1 will let it continue
    */
    if (strcmp(input, "exit") == 0) {
        if (DEBUG == 1) {
            char* pid = malloc(sizeof(char) * 10);
            sprintf(pid, "PID: %d", getpid());
            DebugPrint("Exiting program with 0:", pid);
        }
        return 0;
    }
    else {
        // TODO: handle commands

        // ideas:
        //  - probably break into separate function
        //  - use strtok to split input into tokens (tokenize the input)
        //  - look for special characters like >, <, |, etc. (find redirects and redirect status)
        //  - if no special characters, just run the command (run command)
        //  - if special characters, run the command and redirect the output (RedirectOutput)
        //    or run command with redirected input (RedirectInput)

        return 1;
    }
}