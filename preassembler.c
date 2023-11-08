#include "macro.h"
#include "errors.h"
#include <stdio.h>
#include <string.h>
#define MAXLINE  81

int preassemble(char *file){
    /*declerations and initializing*/
    int error=0;
    char filename[MAXLINE + 1], line[MAXLINE + 2] ;
    int linenum=0;
    FILE *source, *target;
    Macro mcro;
    Macrolist macrolist= newmacrolist();
    strcpy(filename,file);
    source = fopen(strcat(filename, ".as"),"r");
    strcpy(filename,file);
    target= fopen(strcat(filename, ".am"),"w");

    /*go line by line*/
    while (fgets(line, sizeof line, source) != NULL){
        char firstent[MAXLINE + 1]="", secondent[MAXLINE + 1]="",thisrdent [MAXLINE + 1]=""; /*initialise buffers*/
        linenum++;/*count lines*/
        if (strlen(line) > MAXLINE || ((strlen(line) == MAXLINE && line[MAXLINE - 1] != '\n'))){
            errlinetoolong(linenum);
            error ++;
        }

        sscanf(line,"%s %s %s",firstent,secondent,thisrdent);
        /*macro definition:*/
        if (strcmp(firstent,"mcro")==0){
            /*ADD LABEL CHECKS*/
            if (strcmp(thisrdent,"")!=0){
                error++;
                errextratxt(linenum);
            } else if (islegallabel(secondent, linenum)&&!ismacroexist(macrolist,secondent)) {
                addmacro(macrolist, secondent);
                mcro = getmacro(macrolist, secondent);
                while (fgets(line, sizeof line, source) != NULL) {
                    firstent[0]=secondent[0]='\0'; /*reset buffers*/
                    linenum++;
                    sscanf(line, "%s %s", firstent, secondent);
                    /*definition ended*/
                    if (strcmp(firstent, "endmcro") == 0) {
                        if (strcmp(secondent,"")!=0){
                            error++;
                            errextratxt(linenum);
                        }
                        break;
                    } else { /*command is part of mcro - add to data struct*/
                        addcommand(mcro, line);
                    }
                } /*end macro while loop*/
            }else{
                error++;
                if (ismacroexist(macrolist,secondent)){
                    errmcroalrdyxsist(linenum, secondent);
                }
            }
        }/*end macro*/

        /*case mcro implementation*/
        else if (ismacroexist(macrolist, firstent)){
            if (strcmp(secondent,"")!=0){
                error++;
                errextratxt(linenum);
            }
            mcro = getmacro(macrolist, firstent);
            putmacro(mcro, target);
        } else{/*line unrelated to mcro*/
            fputs(line, target);
        }

    }
    macrolistfree(macrolist);
    fclose(source);
    fclose(target);
    return error;
}
