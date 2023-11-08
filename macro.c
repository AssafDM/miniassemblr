#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

struct Macro {
    char* name;
    StringList commands;
};
struct Macrolist{
    Macro macro;
    struct Macrolist *next;
};

/*initialise a new macro with a new name*/
Macro newmacro( char *name) {
    Macro macro = (Macro) malloc(sizeof(struct Macro));


    macro ->name = (char *) malloc(sizeof (char )* strlen(name)+1);
    /*put name and set a null command list*/
    strcpy(macro ->name, name);
    macro->commands = newlist();
    return macro;
}
/*add a command line to a mcro*/
void addcommand(Macro macro, char *string) {
        /*add command*/
        addstring(macro->commands, string);
}
/*writes macro to file line by line*/
void putmacro(Macro macro, FILE *file) {
    listput(macro->commands, file);
}
/*frees all memory alocations made for macro*/
void macrofree(Macro macro) {
    if (!(macro==NULL)){
        free(macro->name);
        listfree(macro->commands);
        free(macro);
    }

}
 /*macrolist functions*/
 /*creates a new empty macro list*/
Macrolist newmacrolist(void){
     Macrolist new = (struct Macrolist*) malloc(sizeof (struct Macrolist ));
     new->macro=NULL;
     new->next=NULL;
     return new;
}
/*adds a macro to macro list*/
void addmacro(Macrolist list, char *name) {
    if (list->macro ==NULL){
        list->macro= newmacro(name);
        list->next=NULL;
    }
    else{
        Macrolist current= list;
        Macrolist new= newmacrolist();
        while (current->next!=NULL){
            current= current->next;
        }
        current->next = new;

        new->next= NULL;
        new->macro= newmacro(name);

    }
}
/*find a macro with a given name in the list, returns null if not found*/
Macro getmacro(Macrolist list, char *name) {
    Macrolist current= list;
    while (current!=NULL){
        if (strcmp(current->macro->name, name)==0){
            return current->macro;
        } else{
            current=current->next;
        }
    }
    return NULL;
}
/*returns 1 if a macro exist in list, 0 otherwise*/
int ismacroexist(Macrolist list, char *name) {
    Macrolist current= list;
    while (current!=NULL&&current->macro!=NULL){
        if (strcmp(current->macro->name, name)==0){
            return 1;
        } else{
            current=current->next;
        }
    }
    return 0;
}
/*frees all macros and memory allocations in list*/
void macrolistfree(Macrolist list) {
    if (list==NULL){
        return;
    } else{
        macrolistfree(list->next);
        macrofree(list->macro);
        free(list);
    }
}

