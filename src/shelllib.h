#ifndef SHELLLIB_H
#define SHELLLIB_H

// ENUMS and GLOBAL CONSTANTS
extern int MAX_PATH;
extern int DEBUG;
extern char* GREEN;
extern char* NORMAL;
extern char* RED;
enum status;

// UTILITIES
void trimLeading(char* input);
void trimTrailing(char* input);
void trimWhitespace(char* input);

// DEBUGGING TOOLS
void DebugPrint(char* message, char* data);
void ErrorPrint(char* message, char* data);


// GENERATORS
char* makePrompt();

// PARSERS
enum status tokenize(char input[256]);

// HANDLERS
int handleNoRedirect(char* input);
int handleRedirectInput(char* input);
int handleRedirectOutput(char* input);
int handleInput(char* input);

#endif