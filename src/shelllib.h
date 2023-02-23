#ifndef SHELLLIB_H
#define SHELLLIB_H

// ENUMS and GLOBAL CONSTANTS
enum status;

// DEBUGGING TOOLS
void DebugPrint(char* message, char* data);
void ErrorPrint(char* message, char* data);

extern int MAX_PATH;
extern int DEBUG;
extern char* GREEN;
extern char* NORMAL;
extern char* RED;

// GENERATORS
char* makePrompt();

// PARSERS
enum status tokenize(char input[256]); // return could be status enum?
//int tokenize(char input[256]); // could also return number of status enum

// HANDLERS
int handleNoRedirect(char* input);
int handleRedirectInput(char* input);
int handleRedirectOutput(char* input);
int handleInput(char* input);

#endif