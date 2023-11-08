#include "util.h"
#include <stdio.h>
#ifndef Macro_h
#define Macro_h

typedef struct Macro* Macro;
typedef struct Macrolist* Macrolist;
#endif

void addmacro(Macrolist list , char *name);
Macro getmacro(Macrolist list, char *name);
int ismacroexist(Macrolist list, char *name);
void macrolistfree(Macrolist list);
Macrolist newmacrolist(void);




Macro newmacro( char *name);
void addcommand(Macro macro, char *string);
void putmacro(Macro macro, FILE*file);
void macrofree(Macro macro);
