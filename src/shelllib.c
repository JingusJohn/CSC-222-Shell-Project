#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<errno.h>

#define MAX_PATH 256
#define DEBUG 1
#define GREEN "\x1B[32m"
#define NORMAL "\x1B[0m"
#define RED "\x1B[31m"

//ENUMS AND GLOBAL CONSTANTS
enum status {NoRedirect, RedirectInput, RedirectOutput, UnsupporedRedirect, Error, ChangeDirectory};

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

// GENERATORS
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

int handleRedirectInput(char *input) {
    // implement
    return 0;
}

int handleRedirectOutput(char *input) {
    // split string along the redirect character (gets left side first)
    char *leftSide = strtok(input, ">");
    // get the right side of the string
    char *rightSide = strtok(NULL, ">");

    // create new child process to execute command
    pid_t child = fork();
    if (child == -1) {
        // handle errors
        // return -1 to be handled further
        return -1;
    } else if (child == 0) {
        // execute command in child process
        // open file for writing (create if doesn't already exist)
        int file = open(rightSide, O_WRONLY | O_CREAT, 0777);
        // redirect STDOUT (position 2 in file descriptor table) to file [see man page for dup2]
        dup2(file, 1);
        // execute command
        system(leftSide);
        // close file
        close(file);
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
            // cd command
            case ChangeDirectory:
                if (DEBUG == 1) {
                    DebugPrint("redirectStatus:", "ChangeDirectory");
                }
                // handle cd command
                int cmdStatusCd = handleChangeDirectory(input);
                if (cmdStatusCd == -1) {
                    // add string with error number
                    char* error = malloc(sizeof(char) * 40);
                    sprintf(error, "Command: [\"%s\"], ErrorNumber: %d", input, errno);
                    ErrorPrint("Error changing directory:", error);
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

