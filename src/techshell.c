#include<stdio.h>
#include "shelllib.h"
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define MAX_PATH 256
#define DEBUG 1

int main() {
  // program will run while running == 1
  //  running will be set to 0 if the user inputs "exit" or some sort of error is encountered
  int running = 1;
  while (running == 1) {
    // generate prompt and print it to user without newline
    char* prompt = makePrompt();
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
