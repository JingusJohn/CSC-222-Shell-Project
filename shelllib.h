#ifndef SHELLLIB_H
#define SHELLLIB_H

// GENERATORS
char* makePrompt();

// PARSERS
enum status tokenize(char input[256]); // return could be status enum?
//int tokenize(char input[256]); // could also return number of status enum

// DEBUGGING TOOLS
void DebugPrint(char* message, char* data);

// HANDLERS
int handleInput(char* input);

// ENUMS and GLOBAL CONSTANTS
enum status {NoRedirect, RedirectInput, RedirectOutput, RedirectAppend, RedirectError, Error};

extern int MAX_PATH;
extern int DEBUG;
extern char* GREEN;
extern char* NORMAL;

#endif