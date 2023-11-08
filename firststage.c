#include "datalist.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "firststage.h"

#define MAXLINE 81
#define MAXLABEL 31
#define IC_START 100
#define DC_START 0

/*this method implements the first stage of assembly, it returns the number of data */
int firstStage(char *filename, datalist data, labellist labels) {
    /*declarations*/
    char *fileaddres = malloc(sizeof(char) * (strlen(filename) + 4));
    char *fgetsval, line[MAXLINE + 2];
    char *temp;
    int ic = IC_START, dc = DC_START, linenum = 0, err = 0;
    FILE *file;
    datalist currentdata =data;
    order order;
    /*end declarations*/
    /*open file to read*/
    strcat(strcpy(fileaddres, filename), ".am");
    file = fopen(fileaddres, "r");

    /*read each line*/
    while ((fgetsval = fgets(line, MAXLINE + 1, file)) != NULL) {
        linenum++;
        if (fgetsval != NULL && !isinvisible(line)) {
            /*we now have a real line*/
            if (strlen(line) > MAXLINE || ((strlen(line) == MAXLINE && line[MAXLINE - 1] != '\n'))){
                errlinetoolong(linenum);
                err++;
            }else {
                order = neworder(line, linenum);

                /*if its a data/string  order with a label*/

                    if (getlabel(order) != NULL && (strcmp(getcommand(order), ".data") == 0 ||
                                                    strcmp(getcommand(order), ".string") == 0)) {
                        if (!islegallabel(getlabel(order), linenum)) {
                            err++;
                        }else if (addlabel(labels, getlabel(order), getcommand(order), dc, getlinenum(order)) == 0) {
                            int success = writedata(order, data);
                            /*writing data encountered errors*/
                            if (success < 0) {
                                dc -= success; /*illegal vars, but words were loaded to datalist*/
                                err++;

                            /*writing data was successful*/
                            } else if (success > 0) {
                                dc += success;

                            /*no data was written, write dummy data to avoid errors*/
                            } else {
                                currentdata = additem(currentdata);
                                setdata(currentdata, temp=tobinary(0, 12));
                                free(temp);
                                dc++;
                                err++;

                            }

                        /*label illegal or already exists*/
                        } else { err ++; }


                }/*end data/string labels*/
                /*the command is unknown*/
                else if (iscommand(getcommand(order)) == -1) {
                    errunknowencmd(linenum , getcommand(order));
                    err++;
                    if (getlabel(order) != NULL) {
                        addlabel(labels, getlabel(order), getcommand(order), ic++, getlinenum(order));
                    }

                /*regular command (mov-stop)*/
                } else if (iscommand(getcommand(order)) <= 15) {/*if command known we can continue*/
                    if (getlabel(order) != NULL) {
                        if (addlabel(labels, getlabel(order), "code", ic, getlinenum(order)) != 0) {
                            err++;
                        }
                    }
                    ic++;
                    /*check legality of operands and calculate the number of added words to instruction*/
                    if (areoperandslegal(order)) {
                        int *addcodes = getaddcode(order);
                        if (addcodes[0] == 5 && addcodes[1] == 5) {
                            ic++; /*add one word*/
                        } else {
                            int i;
                            for (i = 0; i < getnumberofvars(order); ++i) {
                                ic++; /*add a word for each operand*/
                            }
                        }
                        free(addcodes);
                    }else{

                        err++; /*operands ilegal*/
                    }
                }
                /*case data without label*/
                else if (strcmp(getcommand(order), ".data") == 0 || strcmp(getcommand(order), ".string") == 0) {
                    int added = writedata(order, data);
                    if (added <= 0) {
                        err++;
                    }
                    dc += abs(added); /*add count of added values to dc*/
                    /*case extern command*/
                } else if (strcmp(getcommand(order), ".extern")==0) {
                    int i;
                    if (getlabel(order) != NULL) {
                        wrnredundantlabel(getlinenum(order));
                    }
                    if (getnumberofvars(order)==0){
                        err++;
                        errilegalvarnum(getlinenum(order),1, getnumberofvars(order));
                    }
                    for (i = 0; i < getnumberofvars(order); i++) {
                        if (addlabel(labels, getvars(order)[i], ".extern", 0, getlinenum(order)) != 0) {
                            err++;
                        }
                    }
                    /*setting as entry is saved for stage two, just check legality.*/
                } else if (strcmp(getcommand(order), ".entry")==0) {
                    if (getlabel(order) != NULL) {
                        wrnredundantlabel(getlinenum(order));
                    }
                        if (getnumberofvars(order)==0){
                            err++;
                            errilegalvarnum(getlinenum(order),1, getnumberofvars(order));
                        }
                }


                freeorder(order);
            }/*end if too long*/
        }/*end if not emtpy*/
    }/*endwhile*/
    free(fileaddres);
    shiftAddress(data, ic);/*update the data values according to the*/
    shiftdatalabels(labels, ic);
    fclose(file);
    if (err > 0) {
        return -err;
    } else {
        return dc;
    }

}

int writedata(order order, datalist data) {
    int newwordcnt = 0, err = 1;/*if err is positive there is no error, if negative theres error*/
    char *temp;
    /*data order*/
    if (strcmp(getcommand(order), ".data") == 0) {
        int i;
        datalist newdata = data;
        if (getnumberofvars(order)==0){
            err = -1;
            errilegalvarnum(getlinenum(order), 1, 0);
        }
        for (i = 0; i < getnumberofvars(order); i++) {
            if (isnumber(getvars(order)[i])) {
                newdata = additem(newdata);
                setdata(newdata, temp=tobinary(atoi(getvars(order)[i]), 12));
                free(temp);
                newwordcnt++;
            } else {
                err = -1;
                errnotanumber(getlinenum(order),getvars(order)[i] );
            }

        }

        return newwordcnt * err;
    } else {
        int counter = 0;
        datalist newdata = data;
        char *current = getvars(order)[0];
        char *start = NULL, *end = NULL;
        while (*current) {
            if (*current == '\"') {
                if (start == NULL) {
                    start = current;
                } else { end = current; }
            } else if (!isspace(*current)) {
                if (start == NULL) { break; } /*this operation makes sure that the first visible char is '"'*/
                end = NULL; /*this operation guarantee that if the last visible char is not '"' the string will not be accepted*/
            }
            current++;
        }
        /*string does not end and start in '"'*/
        if (end == NULL) {

            newdata = additem(newdata);
            setdata(newdata, temp=tobinary(0, 12));
            free(temp);
            errilegalstring(getlinenum(order));
            return -1;
        }
        current = start + 1;
        while (current < end) {
            /*case ilegal char*/
            if(!(*current>=32&& *current<=126)){/*char is not printable (printable dafined for classic asci chars only in this program*/
                err=-1;
                errilrgalchar(getlinenum(order));
                current=end;
            }else {
                newdata = additem(newdata);
                setdata(newdata, temp = tobinary(*current, 12));
                free(temp);
                counter++;
                current++;
            }
        }
        /*add '\0'*/
        newdata = additem(newdata);
        setdata(newdata, temp=tobinary(0, 12));
        free(temp);
        return ++counter*err;
    }


}
