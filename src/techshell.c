/*-----------------------------------------------------
Techshell
Authors: Jack Branch, Zak Kilpatrick

Created for CSC-222 Programming Project

Techshell can execute simple commands with two types of
 I/O redirection.
 ">" redirects output to a file
 "<" redirects input from a file

All functions and constants in the shelllib have been
 given doc comments to explain their purpose. The two
 globals here are quite self-explanatory.

Set DEBUG to 1 to enable debug printing.
-----------------------------------------------------*/

#include <stdio.h>
#include "shelllib.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_PATH 256
#define DEBUG 0

int main()
{
    // program will run while running == 1
    //  running will be set to 0 if the user inputs "exit" or some sort of error is encountered
    int running = 1;
    if (DEBUG == 1) {
        DebugPrint("DEBUGGING IS ENABLED", "");
    }
    while (running == 1) {
        if (DEBUG == 1) {
            // pid has a max of 32768.
            // make empty string for pid. "PID: " + 5 digits + null terminator
            char pid[11];
            sprintf(pid, "PID: %d", getpid());
            DebugPrint("Prompted with process:", pid);
        }
        // generate prompt and print it to user without newline
        char *prompt = makePrompt();
        printf("%s ", prompt);
        // after printing the prompt, free the memory allocated for it since it's no longer needed
        free(prompt);

        // get user input
        char input[MAX_PATH];
        fgets(input, MAX_PATH, stdin);
        // remove trailing newline from input
        input[strcspn(input, "\n")] = 0;

        if (DEBUG == 1) {
            DebugPrint("user inputted:", input);
        }
        // determine whether or not to continue running based on output from handler function
        running = handleInput(input);
    }
    return 0;
}
