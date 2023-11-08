#include "stdio.h"
#include "errors.h"

#define errmsg(type, line) printf("Error! %s in line %d.\n", type, line)
void  errcommasaperation(int line){
    errmsg("Ilegal comma placement", line);
}
void  errilegalLabel(int line, char* lbl){
    printf("Error! Label \"%s\" in line %d is an illegal label. all labels must be up to 31 charachters and start with a letter.\n",lbl,line);
}
void  errunknowencmd(int line, char* cmd){
    printf("Error! Command %s in line %d is undefined\n", cmd, line);
}
void  wrnredundantlabel(int line){
    errmsg("Redundent Label",line);
}
void  errnotanumber(int line, char *str){
    printf("Error! Variable \"%s\" in line %d is no an integer.\n", str,line );

}
void  errlabelalreadyexist(int line, char* lbl){
    printf("Error! Label %s  in line %d is already defined\n", lbl,line);
}
void  errilegalstring(int line){
    errmsg("Ilegal string definition", line);
}
void errlblnfound(char *lbl, int line){
    printf("Error! the label \"%s\" used in line %d is not defined.\n", lbl, line);
}
void  errilegalreg(int line){
    errmsg("Register does not exist", line);
}

void errilegalvarnum(int line, int supposetobe, int exists) {
    printf("Error!  in line %d: expected %d variables but received %d\n",line,supposetobe,exists);
}

void errlinetoolong(int line) {
    errmsg("too mant charachters",line);
}

void errmcroalrdyxsist(int line, char *name) {
    printf("Error! macro %s in line %d already defined.\n", name, line);
}

void errextratxt(int line) {
    errmsg("extra text in the end of the line", line);
}

void errwrongtype(int line, int varnum) {
    printf("Error! in line %d: the %s variable is of wrong .\n",line, varnum==1? "first":"second" );

}

void errilrgalchar(int line) {
    errmsg("Illegal character in string definition", line);
}

void errfilenotexist(char *file){
    printf("Error! the file \"%s\" was not found\n", file);
}