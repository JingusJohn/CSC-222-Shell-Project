#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<errno.h>


//ENUMS AND GLOBAL CONSTANTS
/* Maximum characters to be allocated for a path in prompt */
#define MAX_PATH 256
/* Enables DEBUG print statements at runtime (0: off, 1: on) */
#define DEBUG 0
// ANSI color codes
/* Text following will be green */
#define GREEN "\x1B[32m"
/* Text following will be default */
#define NORMAL "\x1B[0m"
/* Text following will be red */
#define RED "\x1B[31m"
/* ENUM: Status of a tokenized input */
enum status {NoRedirect, RedirectInput, RedirectOutput, UnsupporedRedirect, Error, ChangeDirectory};

// UTILITIES
/* Utility: Trims leading whitespace from string */
void trimLeading(char* input) {
    // count the number of leading whitespace characters (spaces and tabs)
    //  newline characters would just mark the end of the input and be cut out by fgets
    int leading = 0;
    while (input[leading] == ' ' || input[leading] == '\t') {
        leading++;
    }
    if (leading > 0) {
        // this means whitespace was detected
        int pos = 0;
        while (input[pos + leading] != '\0') {
            // shift characters to the left by the amount of leading whitespaces detected
            input[pos] = input[pos + leading];
            pos++;
        }
        // must shift the null terminator as well!
        input[pos] = '\0';
    }
    // if no leading whitespace was detected, do nothing

    // nothing to return. Just modifying the original string
}

/* Utility: Trims trailing whitespace from string */
void trimTrailing(char* input) {
    // start at the end of the string (not including null terminator)
    int pos = strlen(input) - 1;

    while (pos > 0) {
        // if the character is a space or a tab, decrement pos
        if (input[pos] == ' ' || input[pos] == '\t') {
            pos--;
        } else {
            // if the character is not a space or tab, stop. We've reached non-whitespace
            break;
        }
    }
    // move null terminator to the right of the last non-whitespace character (pos + 1)
    input[pos + 1] = '\0';
}

/* Utility: Trims whitespace from either side of string */
void trimWhitespace(char* input) {
    // use trimLeading and trimTrailing helper functions to trim the whitespace
    //  from both sides. No return required since we're just modifying the original string
    trimLeading(input);
    trimTrailing(input);
    // I think this has the potential for memory inefficiencies, but it should work fine
    //  and its potential leakiness is very small since it's deallocated anyways after the
    //  command is executed
}

// DEBUGGING TOOLS
/* Debugging Tool: Used to print various information throughout the runtime */
void DebugPrint(char* message, char* data) {
    /*
    Only for Debugging purposes. Allows for some formatted text
    output. Makes distinction of (DEBUG) prints from normal output
    a bit clearer on the eyes.
    */
    printf("%s(DEBUG)%s %s \"%s\"\n", GREEN, NORMAL, message, data);
}

/* Debugging Tool: Used to print error to user/debugger */
void ErrorPrint(char* message, char* data) {
    /*
    Only for Debugging purposes. Allows for some formatted text
    output. Makes distinction of (ERROR) prints from normal output
    a bit clearer on the eyes.
    */
    printf("%s(ERROR)%s %s \"%s\"\n", RED, NORMAL, message, data);
}

// GENERATORS
/* Generator: Makes terminal prompt for user and includes the working directory */
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
/* Parser: Tokenizes input and decides whether or not user provided a redirect token */
enum status tokenize(char input[256]) {
    // tokenize first substring until whitespace
    char *token = strtok(input, " ");

    // check if first token is cd
    if (strcmp(token, "cd") == 0) {
        return ChangeDirectory;
    }

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
/* Handler: Handles the case where the user changes directory via cd */
int handleChangeDirectory(char *input) {
    // tokenize first substring until whitespace
    
    // split by space
    char *cd = strtok(input, " ");

    char *location = strtok(NULL, " ");

    if (DEBUG == 1) {
        DebugPrint("changing directory: to", location);
    }
    // chdir returns -1 for error and 0 for success
    return chdir(location);
}

/* Handler: Handles the case where the user does not redirect any I/O */
int handleNoRedirect(char *input) {
    // fork child process
    pid_t child = fork();
    if (child == -1) {
        // handle errors
        // return -1 to be handled further
        return -1;
    } else if (child == 0) {
        // child process
        if (DEBUG == 1) {
            DebugPrint("child process: executing command", input);
        }
        // execute command
        system(input);

        if (DEBUG == 1) {
            DebugPrint("child process:", "command executed");
        }
        // exit child process
        exit(0);
    }
    if (DEBUG == 1) {
        DebugPrint("parent process:", "waiting for child process to finish");
    }
    // wait for child process to finish
    wait(NULL);
    if (DEBUG == 1) {
        DebugPrint("parent process:", "child process finished. Continuing");
    }
    return 0;
}

/* Handler: Handles case where user redirects some file as input */
int handleRedirectInput(char *input) {
    // split string along the redirect character (gets left side first)
    char *leftSide = strtok(input, "<");
    // get the right side of the string
    char *rightSide = strtok(NULL, "<");
    // trim whitespace from left and right of the right side of the command
    trimWhitespace(rightSide);

    int signalStatus = 0;

    // create new child process to execute command
    pid_t child = fork();
    if (child == -1) {
        // handle errors
        // return -1 to be handled further
        return -1;
    } else if (child == 0) {
        // execute command in child process
        // open file for reading
        FILE* file = fopen(rightSide, "r");
        // if something goes wrong with open, return 2 signalling that the file doesn't exist
        if (file == NULL) {
            // add string with error number
            char* error = malloc(sizeof(char) * 40);
            sprintf(error, "ErrorNumber: %d", errno);
            ErrorPrint("Couldn't open file for input. Check that it exists!", error);
            exit(0);
        }
        // redirect STDIN (position 0 in file descriptor table) to file [see manpage for dup2]
        dup2(fileno(file), 0);
        // execute command
        system(leftSide);
        // close file
        fclose(file);
        // exit child process
        exit(0);
    }

    // wait for child process to finish
    wait(NULL);
    if (DEBUG == 1) {
        DebugPrint("parent process:", "child process finished. Continuing");
    }

    return 0;
}

/* Handler: Handles case where user redirects the output of the command to some file */
int handleRedirectOutput(char *input) {
    // split string along the redirect character (gets left side first)
    char *leftSide = strtok(input, ">");
    // get the right side of the string
    char *rightSide = strtok(NULL, ">");
    // trim whitespace from left and right of the right side of the command
    trimWhitespace(rightSide);

    // create new child process to execute command
    pid_t child = fork();
    if (child == -1) {
        // handle errors

        // add string with error number
        char* error = malloc(sizeof(char) * 40);
        sprintf(error, "Command: [\"%s\"], ErrorNumber: %d", input, errno);
        ErrorPrint("Couldn't create child process!", error);
        exit(0);
    } else if (child == 0) {
        // execute command in child process
        // open file for writing (creates if it doesn't already exist)
        FILE* file = fopen(rightSide, "w");
        if (file == NULL) {
            // add string with error number
            char* error = malloc(sizeof(char) * 40);
            sprintf(error, "Command: [\"%s\"], ErrorNumber: %d", input, errno);
            ErrorPrint("Couldn't open file for output!", error);
            exit(0);
        }
        // redirect STDOUT (position 2 in file descriptor table) to file [see man page for dup2]
        dup2(fileno(file), 1);
        // execute command
        system(leftSide);
        // close file
        fclose(file);
        // exit child process
        exit(0);
    }
    if (DEBUG == 1) {
        DebugPrint("parent process:", "waiting for child process to finish");
    }

    // wait for child process to finish
    wait(NULL);

    if (DEBUG == 1) {
        DebugPrint("parent process:", "child process finished. Continuing");
    }
    return 0;

}

int handleInput(char* input) {
    /* Function takes an input and returns a 1 or 0.
    0 will cause the program to end. 1 will let it continue
    */
    if (strcmp(input, "exit") == 0) {
        if (DEBUG == 1) {
            // pid has a max of 32768.
            // make empty string for pid. "PID: " + 5 digits + null terminator
            char pid[11];
            sprintf(pid, "PID: %d", getpid());
            DebugPrint("Exiting program with 0:", pid);
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
            // cd command
            case ChangeDirectory:
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "ChangeDirectory");
                }
                // handle cd command
                int cmdStatusCd = handleChangeDirectory(input);
                if (cmdStatusCd == -1) {
                    // add string with error number (40 is arbitrary)
                    char* error = malloc(sizeof(char) * 40);
                    sprintf(error, "Command: [\"%s\"], ErrorNumber: %d", input, errno);
                    ErrorPrint("Error changing directory. Check that it exists...", error);
                }
                break;
            // workable redirects
            case NoRedirect:
                // run command as normal
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "NoRedirect");
                }
                int cmdStatusNoRe = handleNoRedirect(input);
                if (cmdStatusNoRe == -1) {
                    char* error = malloc(sizeof(char) * 40);
                    sprintf(error, "Command: [\"%s\"], ErrorNumber: %d", input, errno);
                    ErrorPrint("Error executing command:", error);
                }
                break;
            case RedirectInput:
                // run command with redirected input
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "RedirectInput");
                }
                int cmdStatusReIn = handleRedirectInput(input);
                if (cmdStatusReIn == -1) {
                    char* error = malloc(sizeof(char) * 40);
                    sprintf(error, "[\"%s\"] ErrorNumber: %d", input, errno);
                    ErrorPrint("Error executing command:", error);
                }
                break;
            case RedirectOutput:
                // run command and redirect output
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "RedirectOutput");
                }
                int cmdStatusReOut = handleRedirectOutput(input);
                if (cmdStatusReOut == -1) {
                    char* error = malloc(sizeof(char) * 40);
                    sprintf(error, "[\"%s\"] ErrorNumber: %d", input, errno);
                    ErrorPrint("Error executing command:", error);
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

