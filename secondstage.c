#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "datalist.h"

#define MAXLINE 81
#define BITNUM 12
#define IC_START 100


int secondStage(char *filename,  datalist instructions, labellist labels, int numberofregs) {
    char *fileaddres = malloc(sizeof(char) * (strlen(filename) + 4));
    char *fgetsval, line[MAXLINE + 1], **vars;
    char *temp;
    int linenum = 0, err = 0 ,ic = IC_START;
    order order;
    datalist  currentinstruction;
    FILE *file;
    fileaddres = strcat(strcpy(fileaddres, filename), ".am");
    file = fopen(fileaddres, "r");
    currentinstruction = instructions;

    while ((fgetsval = fgets(line, MAXLINE + 1, file)) != NULL) {
        linenum++;
        while (fgetsval != NULL) {
            if (fgetsval != NULL && isinvisible(line)) {
                linenum++;
                fgetsval = fgets(line, MAXLINE + 1, file);
            } else { break; }
        }
        if (fgetsval == NULL) { break; }

        order = neworder(line, linenum);
        if (!(strcmp(getcommand(order), ".string") == 0 || strcmp(getcommand(order), ".data") == 0 ||
              strcmp(getcommand(order), ".extern") == 0)) {

            char *bitword = malloc(sizeof(char) * 13);
            bitword[0] = '\000';

            if (strcmp(getcommand(order), ".entry") == 0) {
                int i ;
                vars = getvars(order);
                for (i = 0; i < getnumberofvars(order); i++) {
                    if (!setentry(labels, vars[i])) {
                        err++;
                        errlblnfound(vars[i], linenum);
                    }
                }
            }/*end entry cmd*/
                /*based on the first stage wh know operand types and number are correct*/
            else  {
                int *addresscode = getaddcode(order),i;
                vars = getvars(order);
                strcat(bitword, temp=tobinary((getnumberofvars(order)==1)? 0: addresscode[0], 3));/*add address code in binary*/
                free(temp);
                strcat(bitword, temp=tobinary(iscommand(getcommand(order)), 4));
                free(temp);
                strcat(bitword, temp=tobinary((getnumberofvars(order)==1)? addresscode[0]:addresscode[1] , 3));/*add address code in binary*/
                free(temp);
                strcat(bitword, "00");/* absolut value*/
                currentinstruction= additem(currentinstruction);
                ic++;
                setdata(currentinstruction, bitword);
                /*case two registers*/
                if(addresscode[0]==5&&addresscode[1]==5){
                    int src, dst;
                    currentinstruction= additem(currentinstruction);
                    ic++;
                    bitword[0]='\000';
                    src= atoi(vars[0]+2);
                    dst = atoi(vars[1]+2);
                    if (!(src>=0&&src<numberofregs&&dst>=0&&dst<numberofregs)){
                        err++;
                        errilegalreg(linenum);
                    }else{
                        strcat(bitword, temp=tobinary(src, 5));
                        free(temp);
                        strcat(bitword, temp=tobinary(dst, 5));
                        free(temp);
                        strcat(bitword, "00");/*absolut value*/
                        setdata(currentinstruction, bitword);

                    }

                }else {

                    for (i = 0; i < getnumberofvars(order); i++) {
                        int address;
                        bitword[0]='\000';
                      /*  if (getnumberofvars(order)==1){i++;} skip source operand*/
                        switch (addresscode[i]) {

                            case 1:/*absolut number*/
                                strcat(bitword, temp=tobinary(atoi(vars[i]), 10));
                                free(temp);
                                strcat(bitword, "00");/*absolut value*/
                                break;
                            case 3:
                                if ((address= getval(labels, vars[i], getaddress(currentinstruction)+1))<0){
                                    err++;
                                    errlblnfound(vars[i], getlinenum(order));
                                }else{
                                    strcat(bitword, temp=tobinary(address,10));
                                    free(temp);
                                    strcat(bitword, temp=tobinary(AREbylabel(labels, vars[i]),2));/*add ARE val*/
                                    free(temp);
                                }
                                break;
                            case 5:/*register*/
                                address = atoi(vars[i]+2);
                                if (!(address>=0&&address<numberofregs)) {/*illegal register*/
                                    err++;
                                    errilegalreg(linenum);
                                }
                                if (i==getnumberofvars(order)-1){ /*it is the destination var*/
                                    strcat(bitword, "00000");
                                    strcat(bitword, temp=tobinary(address,5));
                                    free(temp);
                                    strcat(bitword, temp= tobinary(0,2));/*fill with 0 includes absolute value*/
                                    free(temp);
                                }else{
                                    strcat(bitword, temp=tobinary(address,5));
                                    free(temp);
                                    strcat(bitword, temp= tobinary(0,7));/*fill with 0 includes absolute value*/
                                    free(temp);
                                }


                                break;
                        }
                        currentinstruction= additem(currentinstruction);
                        ic++;
                        setdata(currentinstruction, bitword);
                    }
                }/*end of adding words*/
                free(addresscode);
            }/*end order treatment*/
            free(bitword);
        }
        /*memory cleanup*/
        freeorder(order);
    }/*end main while loop*/
    fclose(file);
    free(fileaddres);
    if (err>0) {
        return -err;
    } else {
        return ic-IC_START;
    }

}
