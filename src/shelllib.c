#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define MAX_PATH 256
#define DEBUG 1
#define GREEN "\x1B[32m"
#define NORMAL "\x1B[0m"
#define RED "\x1B[31m"

//ENUMS AND GLOBAL CONSTANTS
enum status {NoRedirect, RedirectInput, RedirectOutput, UnsupporedRedirect, Error};

// DEBUGGING TOOLS
void DebugPrint(char* message, char* data) {
    /*
    Only for Debugging purposes. Allows for some formatted text
    output. Makes distinction of (DEBUG) prints from normal output
    a bit clearer on the eyes.
    */
    printf("%s(DEBUG)%s %s \"%s\"\n", GREEN, NORMAL, message, data);
}

void ErrorPrint(char* message, char* data) {
    /*
    Only for Debugging purposes. Allows for some formatted text
    output. Makes distinction of (ERROR) prints from normal output
    a bit clearer on the eyes.
    */
    printf("%s(ERROR)%s %s \"%s\"\n", RED, NORMAL, message, data);
}

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

// PARSERS
/* Parses input and decides whether or not user provided a redirect token */
enum status tokenize(char input[256]) {
    // tokenize first substring until whitespace
    char *token = strtok(input, " ");
    int redirectTokenCount = 0;
    enum status redirectStatus = NoRedirect;

    // loop through the string to extract all other tokens
    while (token != NULL &&
    redirectTokenCount < 2 &&
    redirectStatus != UnsupporedRedirect){
        if (DEBUG == 1) {
            DebugPrint("token:", token);
        }
        // check for redirects
        if (strcmp(token, ">") == 0) {
            redirectStatus = RedirectOutput;
            redirectTokenCount++;
        }
        else if (strcmp(token, ">>") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, "<") == 0) {
            redirectStatus = RedirectInput;
            redirectTokenCount++;
        }
        else if (strcmp(token, "<<") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, "|") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, "2>") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, "2>>") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, "&>") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, "&>>") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        else if (strcmp(token, ">>") == 0) {
            redirectStatus = UnsupporedRedirect;
            redirectTokenCount++;
        }
        token = strtok(NULL, " ");
    }
    // if more than one redirect token was found, return error
    if (redirectTokenCount > 1) {
        redirectStatus = Error;
    } else if (redirectTokenCount == 0) {
        // if no redirect tokens were found, return NoRedirect
        redirectStatus = NoRedirect;
    }
    return redirectStatus;
}


// HANDLERS
int handleInput(char* input) {
    /* Function takes an input and returns a 1 or 0.
    0 will cause the program to end. 1 will let it continue
    */
    if (strcmp(input, "exit") == 0) {
        if (DEBUG == 1) {
            char* pid = malloc(sizeof(char) * 10);
            sprintf(pid, "PID: %d", getpid());
            DebugPrint("Exiting program with 0:", pid);
            free(pid);
        }
        return 0;
    }
    else {

        // make a copy of the input to avoid modifying the original
        char inputCopy[256];
        strcpy(inputCopy, input);

        // tokenize the input
        enum status redirectStatus = tokenize(inputCopy);

        // TODO: handle redirects based on status
        // need to verify that the there are arguments before and after the redirect token
        //  can either do that when tokenizing or when handling the redirects (not sure which yet)
        switch (redirectStatus) {
            // workable redirects
            case NoRedirect:
                // run command as normal
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "NoRedirect");
                }
                break;
            case RedirectInput:
                // run command with redirected input
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "RedirectInput");
                }
                break;
            case RedirectOutput:
                // run command and redirect output
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "RedirectOutput");
                }
                break;
            // unsupported redirects and errors
            case UnsupporedRedirect:
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "UnsupportedRedirect");
                }
                ErrorPrint("Unsupported redirect provided:", input);
                break;
            case Error:
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "Error");
                }
                ErrorPrint("Too many redirects provided:", input);
                break;
            default:
                printf("Error: Unknown error.");
                break;
        }
        return 1;
    }
}

