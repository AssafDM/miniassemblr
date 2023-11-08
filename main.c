#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "preassembler.h"
#include "datalist.h"
#include "firststage.h"
#include "secondstage.h"
#include "errors.h"

#define NUMOFREGS 8


int assemble(char *filenamenoend);

int main(int argc, char *argv[]) {
    int i, err=0;
    for (i = 1; i < argc; i++) {
        err+= assemble(argv[i]);
    }


    return err;


}

int assemble(char *filenamenoend) {
    FILE *check;
    char *fullname = malloc((strlen(filenamenoend)+4)*sizeof (char ));
    char *amfile;
    int dc, ic;
    datalist instructions = newdatalist(100), data = newdatalist(0);
    labellist labels = newlabellist();
    
    strcpy(fullname, filenamenoend);
    strcat(fullname, ".as");
    amfile= mystrdup(fullname);
    amfile[strlen(amfile)-1]= 'm';

    check= fopen(fullname, "r");
    /*check if file exists*/
    if (check ==NULL){
        errfilenotexist(filenamenoend);
        return 1; /*file does not exist, return error*/
    }else {
        fclose(check); /*file was found close it*/
    }

        

        if (preassemble( filenamenoend)){/*preassembler encountered errors*/
            remove(amfile);
            return 1;
        }
        free(amfile);
        dc = firstStage(filenamenoend, data, labels);
        if (dc < 0) {
            return 1;
        }
        ic = secondStage(filenamenoend, instructions, labels, NUMOFREGS);
        if (ic < 0) {
            return 1;
        }


        if (combinedata(instructions, data)) { /*if all numbers matched*/
            putdatatofile(filenamenoend, instructions, ic, dc);
            createentext(labels, filenamenoend); /**/
            freedatalist(instructions);
            freelabellist(labels);
            free (fullname);
            return 0;
        } else {freedatalist(instructions);
            freedatalist(data);
            freelabellist(labels);
            free (fullname);
            return 1;
        }



    return 1;


}
