#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "datalist.h"
#include "errors.h"


#define BITNUM 12
struct datalist{
    int address;
    char *data;
    struct datalist *next;
};
/*initialise a new empty datalist starting address count from the given integer*/
datalist newdatalist(int firstslot){
    datalist temp = (datalist )malloc(sizeof (struct datalist));
    temp-> address=firstslot;
    temp ->next = temp; /*this shows that it is uninnitialized*/
    return temp;
}
/*adds a new default value data line to a list, maintaining address count*/
datalist additem(datalist list){
    if (list == NULL){
        return NULL;
    }
    if (list->next ==list){ /*empty list*/
        list->next = NULL;
        list->data = mystrdup("000000000000"); /*default value*/
        return list;
    } else{
        datalist temp;
        datalist current=list;
        /*get to last link*/
        while (current->next!=NULL){
            current= current->next;
        }
        /*create new instance*/
        temp=(datalist) malloc(sizeof(struct datalist));
        current->next=temp;
        temp->address=current->address+1; /*set address*/
        temp->next=NULL;

        temp->data = mystrdup("000000000000"); /*set default val*/

        return temp;
    }

}

/*sets data to a given 12 chars binaty rep of a number*/
int setdata(datalist slot, char * data){
    int i;
    if (strlen(data)!=BITNUM){
        return -1;
    }
    for(i=0; i<BITNUM;i++){
        if (data[i]!='0' &&data[i]!='1'){
            return -1;
        } else{
            slot->data[i] = data[i];
        }
    }
    return 0;
}
/*returns the address of a given datalist instance*/
int getaddress(datalist slot){
    return slot->address;
}
/*write all data list to a file in base 64, with first line being the number of instruction and data in base 10 as given*/
void putdatatofile(char *filename,datalist data, int iblock, int dblock ){
    char *obfile = mystrdup(filename);
    int digits = iblock/10+1+dblock/10+1; /*number of needed digits*/
    char* firstline= malloc(sizeof(char)*(digits+2)); /*number of digits + tab and '\0'*/
    FILE *file;
    datalist current = data;

    /*init file*/
    strcat(obfile, ".ob");
    remove(obfile); /*clear any old files*/
    file= fopen(obfile, "w");
    free(obfile);
    /*empty file, no output*/
    if(data->next== data){
        fclose(file);
        return;
    }
    /*write first line to file*/
    sprintf((char*)firstline, "%d %d\n", iblock, dblock);
    fputs((char*)firstline, file);
    free(firstline);
    /*write all data lines in base64*/
    while (current){
        char *temp=tob64(current->data);
        fputs(temp,file);
        fputs("\n",file);
        free(temp);
        current=current->next;
    }
    fclose(file);

}
/*add a delta value to all addresses in a datalist*/
void shiftAddress(datalist list, int delta){
    datalist current =list;
    while (current!=NULL&&list->next!=list){
        current->address= current->address+delta;
        if (current->address>1024){
            printf("Error: stack overload");
        }
        current= current->next;
    }
}
/*combine two datalist to one while checking last and first values are consecutive*/
int combinedata(datalist dest, datalist toadd){
    datalist current = dest;
    if (current->next ==current) { /*first lisr is empty list*/
        /*make destination a copy of the source*/
        current->address= toadd->address;
        current->data=toadd->data;
        current->next=toadd->next;
        /*free only the memory alocation to the first pointer in the source, all data should be kept*/
        free(toadd);
        return 1;
    }
    /*if first an second list arenot empty append the source to the destination*/
    else if (!(toadd->next ==toadd)){ /*second is not empty*/
        while (current->next){
            current=current->next;
        }
        if (!(current->address== toadd->address-1)){return 0;}/*wrong ic-dc counting*/
        current->next = toadd;
    }else{/*source list is empty*/
        freedatalist(toadd); /*free data allocation*/
    }

    return 1;
}
/*frees all memo allocations in a list*/
void freedatalist(datalist list){
    if (list->next==list){
        free(list);
        return;
    }
    if(list->next!=NULL) { /*not the end of a list / list not empty*/
        if (list->next!=list) {
            freedatalist(list->next);
        }
    }
    free(list->data);

}



/*******************************************************************************/
struct labellist{
    char *label;
    char *type;
    int value;
    int entry;
    int* extmentions;
    struct labellist *next;
};

/*creates a new empty label list*/
labellist newlabellist(void){
    labellist temp = (labellist )malloc(sizeof (struct labellist));
    temp->label=NULL; /*this shows that it is uninitialize*/
    temp->type=NULL;
    temp->next=NULL;
    temp->extmentions=NULL;
    return temp;
}
/*adds a label to a list, returns 0 if addition was successful, -1 otherwise*/
int addlabel(labellist list, char* lbl, char *type, int val, int linenum){
    labellist current = list, last;
    if (!islegallabel(lbl, linenum)){
        return -1;
    }

    if (current->label!=NULL){
        while (current!=NULL){
            if (strcmp(current->label, lbl)==0){
                errlabelalreadyexist(linenum, lbl);
                return -1;
            }
            last =current;
            current=current->next;
        }
        last->next = newlabellist();
        current= last->next;
    }
    current->label=mystrdup(lbl);
    current->type= mystrdup(type);
    current->value= val;
    current->next=NULL;
    current->entry = 0;
    current->extmentions= malloc(sizeof (int)); /*initialise array to be of length 1*/
    current->extmentions[0] = 0; /*the first num in this array represent the number of mentions*/
    return 0;
}
/*returns the address of a label, return -1 if it does not exist*/
int getval(labellist list, char* lbl, int address){
    labellist current = list;
    while (current!=NULL&&current->label!=NULL){
        if(strcmp(current->label, lbl)==0) {
            if (strcmp(current->type, ".extern")==0){
                current->extmentions = realloc(current->extmentions, ((++current->extmentions[0])+1)*sizeof (int));
                current->extmentions[current->extmentions[0]]= address;
            }
            return current->value;
        }
        current= current->next;
    }
    return -1;
}
/*setd a lebel as entry, return 0 if it wasnt found*/
int setentry(labellist list, char *lbl){
    labellist current = list;
    while (current!=NULL&&current->label!=NULL){
        if(strcmp(current->label, lbl)==0) {
            current->entry = 1;
            return 1;
        }
        current= current->next;
    }
    return 0;
}
/*adds a delta to all non external address values in a label list*/
void shiftdatalabels(labellist list, int delta){
    labellist current = list;
    while (current!=NULL&&current->label!=NULL){
        if (strcmp(current->type, ".data")==0||strcmp(current->type, ".string")==0){
            current->value+=delta;
        }
        current=current->next;
    }
}
/*frees all datta allocations in alabel list*/
void freelabellist(labellist list){
    if(list->next!=NULL) {
        freelabellist(list->next);
    }
    free(list->label);
    free(list->type);
    free(list->extmentions);
    free(list);
}
/*this method returns a labels  a,r,e encoding value, returns -1 if not found*/
int AREbylabel(labellist list, char *lbl){
    labellist current = list;
    /*all labels are relocatable or external, absolut is only for immidiate values*/
    while (current!=NULL&&current->label!=NULL){
        if(strcmp(current->label, lbl)==0) {
            if (strcmp(current->type,".extern")==0){
                return 1;
            }else return 2;
        }
        current= current->next;
    }
    return -1;
}
/*this method creates the .ent and .ext files from a label list. only if its is needed*/
void createentext(labellist list, char *filename){
    labellist current = list;
    /*initialize file names*/
    char *entfilename = malloc(sizeof (char )* (strlen(filename)+5));
    char *extfilename = malloc(sizeof (char )* (strlen(filename)+5));
    strcpy(extfilename, filename);
    strcat(extfilename, ".ext");
    strcpy(entfilename, extfilename);
    entfilename[strlen(extfilename)-2]='n';

    /*if exists: old files will be removed*/
    remove(extfilename);
    remove(entfilename);

    if (current->label==NULL){return;}
    /*for each label check if its external and add all mentions to ext file, if its entry add it to ent file*/
    while (current){
        if (strcmp(current->type,".extern")==0){
            int i;
            for(i =1 ; i<= current->extmentions[0] ;i++){
                FILE *file= fopen(extfilename, "a");
                char line [100];
                sprintf(line, "%s\t%d\n", current->label, current->extmentions[i]);
                fputs(line, file);
                fclose(file);
            }
        }
        if (current->entry){
            FILE *file= fopen(entfilename, "a");
            char line [100];
            sprintf(line, "%s\t%d\n", current->label, current->value);
            fputs(line, file);
            fclose(file);

        }
        current = current->next;
    }
    free(extfilename);
    free(entfilename);
}
