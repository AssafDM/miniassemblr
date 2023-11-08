#include <stdio.h>
#ifndef StringList_h
#define StringList_h
typedef struct StringList* StringList;
typedef struct order* order;
#endif

StringList newlist(void);

void addstring(StringList list, char *str);

void listput(StringList string, FILE *file);

void listfree(StringList list);


int islegallabel(char* lbl , int line);
int isnumber(char *str);
char * tobinary(int twoscomp, int len);
char * tob64(char* str);
int islegalfilename(char *name);
int isinvisible(char *str);
char* getfirstword(char* str);
int iscommand(char *string);
int normalizeVars(char *ops, int linenum);
order neworder(char* line,int linenum);
void freeorder(order order);
char* getlabel(order order );
char* getcommand(order order );
int getnumberofvars(order order );
char** getvars(order order );
int getlinenum(order order);
int* getaddcode(order order);
int areoperandslegal(order order);
char *mystrdup(char *str);