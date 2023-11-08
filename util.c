#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "util.h"
#include "errors.h"
#define MAXLINE 81

struct StringList{
    char*data;
    struct StringList *next;
};
/*string list commands*/
/*initialize a new empty list*/
StringList newlist(void) {
    StringList list = (struct StringList *) malloc(sizeof (struct StringList));
    list->data=NULL;
    list-> next = NULL;
    return list;

}
/*adds a string to a stringlist*/
void addstring(StringList list, char *str) {
    StringList current = list;
    /*case empty list*/
    if (list->data==NULL){
        list->data= mystrdup(str);
        return;
    }
    /*get to the last string in list*/
    while (current->next!= NULL){
        current= current->next;
    }
    /*create a new instance and add copy of the string*/
    current->next= (StringList) malloc(sizeof (struct StringList));
    current->next ->data= mystrdup(str);
    current->next-> next = NULL;

}
/*check if label is legal */
int islegallabel(char* lbl , int line){
    int i;
    if (iscommand(lbl)!=-1){
        printf("ERROR: line %d: The label \"%s\" is a saved word\n",line,  lbl);
        return 0;
    }

    if (strlen(lbl)>31){
        errilegalLabel(line, lbl);
        return 0;
    }
    if (!(isupper(lbl[0])|| islower(lbl[0]))){
        errilegalLabel(line, lbl);
        return 0;

    }
    for (i=1;i<strlen(lbl);i++){
        char c = lbl[i];
        if (!(islower(c)|| isupper(c)|| isdigit(c))){
            errilegalLabel(line, lbl);
            return 0;
        }
    }
    return 1;

}

/*writes a string list to file*/
void listput(StringList string, FILE *file) {
    if (string==NULL){
        return;
    }
    fputs(string->data, file);
    listput(string->next, file);
}

/*frees all storage allocation of a given list*/
void listfree(StringList list) {
    if (list==NULL){
        return;
    }
    /*recursive call to all next strings before executing free method*/
    listfree(list->next);
    free(list->data);
    free(list);

}
/*this method get a string representation of a number in binary and converts it to base 64,
 * it requires the number representation to be of multiple of 6 length*/

char* tob64(char* str) {
    int digits = strlen(str)/6;
    /* Define the Base64 characters */
    const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /* Allocate memory for the new string */
    char *result = malloc(sizeof (char)*digits+1);
    int i, j;
    if (strlen(str)%6!=0){return NULL;} /*do not solve for missing values*/
    /* Convert each group of 6 bits to a Base64 character */

    /*for each 6 bit group*/
    for (i = 0; i < digits; i++) {
        int value = 0;
        /*for each character calculate the numerical value by bitwise operation*/
        for (j = 0; j < 6; j++) {
            value = (value << 1) | (str[i * 6 + j] - '0');
        }

        result[i] = base64[value];
    }

    /* Null-terminate the result */
    result[digits] = '\0';

    /* Copy the result back to the original string */
    return result;

}
/*the function assumes the number is lower them ahe maximum value of base 2 in this length,
 * if it is is too big it wil return the lowest bits*/

char* tobinary(int num, int len) {
    /*casting to unsigned turns the number to twos compliment as it is the way its encoded in c*/
    unsigned int twoscomp =(unsigned int) num;
    int i;
    char *binary_str = malloc(sizeof (char)*(len+1));
    if (binary_str == NULL){
        return NULL;
    }

    /* Convert to binary */
    binary_str[len]='\000';
    for (i = len-1; i >= 0; i--) {
        binary_str[i] = (char)('0' + (twoscomp % 2));
        twoscomp = twoscomp / 2;
    }
    return binary_str;
}

/*this method returns 0 if there is a visible character in the string  and 1 if its all invisible or if it start with ";"*/
int isinvisible(char *str){
    int i;
    for(i=0;i< strlen(str);i++){
        if (!isspace(str[i]) && str[i]!=';'){
            return 0;
            /*case comment*/
        } else if(str[i]==';'){
            return 1;
        }
    }
    return 1;

}
/*check if the file name is a legal '.as' file*/
int islegalfilename(char *name){
    int res;
    if (strlen(name)<4){ /*file name need at least one character and .as*/
        return 0;
    }
    res = (strcmp(&name[(int)strlen(name)-3], ".as")==0) ? 1 :0; /*compare the last 3 chars to see if file name right*/
    return res;
}

/*this method returns the code of a command if it exists, -1 otherwise*/
int iscommand(char *string){
    char *actions[]={
            "mov",
            "cmp",
            "add",
            "sub",
            "not",
            "clr",
            "lea",
            "inc",
            "dec",
            "jmp",
            "bne",
            "red",
            "prn",
            "jsr",
            "rts",
            "stop",
            ".data",
            ".string",
            ".entry",
            ".extern"};
    int i;
    for(i=0; i<=19; i++){
        if (strcmp(actions[i], string)==0){
            return i;
        }
    }
    return -1;

}
/*this method return 1 if comma saperation is legal or 0 otherwise*/
int checkCommaSeparation(const char *str) {
    int i = 0;
    int wascomma = 1;
    int empty =1;

    /* Loop through the string */
    while (str[i] != '\0') {

        if (!isspace((unsigned char) str[i])) {
            empty=0;

            /* Check if a comma is present after a word */
            if (str[i] == ',') {
                if (wascomma == 1) {
                    break;
                }
                wascomma = 1;
                /*it is the start of a new word*/
            } else {
                if (wascomma == 0) {
                    return 0;
                }
                wascomma = 0;
                /*get to last character of the word*/
                while (str[i + 1] != ',' && str[i + 1] != '\0' && !isspace(str[i + 1])) {
                    i++;
                }
            }
        }
        i++;
    }
    if(empty==1){
        return 1;
    }
    if (wascomma==1){
        return 0;
    }
    return 1;
}
/*this method delete all spaces and replaces all commas with a tab*/
int normalizeVars(char *ops, int linenum){
    char temp[MAXLINE + 1];
    int i=0, j=0, cnt=0;
    /*if format is ilegal then return -1*/
    if (!checkCommaSeparation(ops)){
       errcommasaperation(linenum);
        return -1;
    }
/*copy only visible characters, replacing commas with tabs*/
    while (ops[i]!='\0'){
       if (!isspace(ops[i])){
           if (ops[i]==','){
               cnt++;
               temp[j++]= '\t';
           } else{
               temp[j++]= ops[i];
           }
       }
       i++;
    }
    /*add string end marker*/
    temp[j]='\0';
    if (strcmp(temp,"")!=0){
        cnt++;
    }
    strcpy(ops, temp);
    return cnt;

}
/*this method return the first word of a string replacing it with spaces in the original string*/
char* getfirstword(char* str){/*returns the firs word in a line of text and replaces it with spaces*/
    char* temp;
    int i =0;
    int l,r;
    /*skip first spaces*/
    while (i<(int)strlen(str)&&(isspace(str[i]))){
        i++;
    }

    l=i;
    /*get to the last character*/
    while (!isspace(str[i])&&i<(int)strlen(str))
    {
        i++;
    }
    r=i-1;
    temp =(char*) malloc(sizeof( char) * (r-l+2));
    for (i=0; i+l < r+1; i++)
    {
        temp[i]=str[i+l];
        str[i+l]=' '; /*replace all read letters with spaces*/
    }
    temp[r+1-l]='\0';
    return temp;

}

/*return 0 if the string does not represent a number, 1 otherwise*/
int isnumber(char *str){
    /*if first digit is not a digit or a sign it is not a number*/
    if (!isdigit(str[0])&&str[0]!='+'&&str[0]!='-'){
        return 0;
    }
    str++;
    while (*str){
        if (!isdigit(*str++)){
            return 0;
        }
    }
    return 1;
}

/*order struct definition*/
struct order {
    int linenum;
    char* label;
    char *command;
    int varsnum;
    char **vars;
};
/*create a new order from a given not invisible line*/
order neworder(char* line,int linenum){
    char* word;
    int i;
    order order= malloc(sizeof(struct order));
    order->label=order->command=NULL;
    order->vars=NULL; /*set defult null value*/
    order->linenum= linenum;
    word= getfirstword(line);
    if (word[strlen(word) - 1] == ':'){ /*label assumes a space after ":"*/
        word[strlen(word) - 1]='\0';

        order->label= word;
        word = getfirstword(line);
    }
    order->command= word;
    /*for any command other than ".string" - get a variables array*/
    if (strcmp(order->command, ".string")!=0) {
        order->varsnum = normalizeVars(line, linenum);
        if (order->varsnum>0) {
            order->vars = malloc(sizeof(char *) * order->varsnum);
            for (i = 0; i < order->varsnum; i++) {
                order->vars[i] = getfirstword(line);
            }
        }
    /*for ".string" make the string the only var in the array*/
    }else{
        order ->varsnum =1;
        order->vars = malloc(sizeof(char *));
        order->vars[0]= mystrdup(line);
    }

    return order;
}
/*frees all orders memory alocations*/
void freeorder(order order){
    int i;
    for ( i = 0; i <order->varsnum ; ++i) {
        free (order->vars[i]);
    }
    free(order->vars);
    free(order->label);
    free(order->command);
    free(order);
}

char* getlabel(order order ){
    return order->label;
}
char* getcommand(order order ){
    return order->command;
}
int getnumberofvars(order order ){
    return order->varsnum;
}
char** getvars(order order ){
    return order->vars;
}
int getlinenum(order order){
    return order->linenum;
}
/*calculate the addressing method of each variable and return an array with the corresponding code*/
int* getaddcode(order order){
    int *res = malloc(sizeof (int)*2), i;
    res[0]=res[1]=0; /*init arr*/
    if (order->varsnum ==0||order->varsnum>2) {return res;}

    for (i=0; i<order->varsnum;i++){
        /*is a number*/
        if (isnumber(order->vars[i])){
            res[i]=1;
            /*if its in the formant of a register*/
        } else if(order->vars[i][0]=='@'&&order->vars[i][1]=='r'&& isnumber(order->vars[i]+2)){
            res[i] = 5;
            /*not number or reg- assume label*/
        } else{
            res [i] = 3;
        }
    }

    return res;
}
/*gets an order and return 1 if the operand types are legal. does not check if the registers and label exist*/
int areoperandslegal(order order){
    int commandcode = iscommand(order->command), res =1;
    int* addcodes = getaddcode(order);
    switch (commandcode) {
        case 0:
        case 2:
        case 3:
            if(order->varsnum !=2){
                res= 0;
                errilegalvarnum(order->linenum, 2, order->varsnum);
            }else {
                if (addcodes[1] == 1) {
                    res = 0;
                    errwrongtype(order->linenum, 2);
                }
            }
            break;
        case 1:
            if(order->varsnum !=2){
                res= 0;
                errilegalvarnum(order->linenum, 2, order->varsnum);
            }
            break;
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 13:
            if(order->varsnum !=1){
                res= 0;
                errilegalvarnum(order->linenum, 1, order->varsnum);
            }
            if (addcodes[0]==1){
                res= 0;
                errwrongtype(order->linenum, 2);
            }
            break;
        case 6:
            if(order->varsnum !=2){
                res= 0;
                errilegalvarnum(order->linenum, 2, order->varsnum);
            }else {
                if (addcodes[0] != 3) {
                    res = 0;
                    errwrongtype(order->linenum, 1);
                }

                if (addcodes[1] == 1) {
                    res = 0;
                    errwrongtype(order->linenum, 2);
                }
            }break;
        case 12:
            if(order->varsnum !=1){
                res= 0;
                errilegalvarnum(order->linenum, 1, order->varsnum);
            }
            break;
        case 14:
        case 15:
            if(order->varsnum !=0){
                res= 0;
                errilegalvarnum(order->linenum, 0, order->varsnum);
            }
            break;

        default: res=  0;
    }
    free(addcodes);
    return res;
}
/*duplicates a string and return pointer to new string*/
char *mystrdup(char *str){
    char* temp= malloc((strlen(str)+1)* sizeof(char));
    strcpy(temp, str);
    return temp;
}





