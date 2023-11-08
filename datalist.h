#include "util.h"
#ifndef datalist_h
#define datalist_h
typedef struct datalist *datalist;
typedef struct labellist *labellist;
#endif
datalist newdatalist(int firstslot);
datalist additem(datalist list);
int setdata(datalist slot, char * data);
int getaddress(datalist slot);
void shiftAddress(datalist list, int delta);
void freedatalist(datalist list);
int combinedata(datalist dest, datalist toadd);
void putdatatofile(char *filename,datalist data, int iblock, int dblock );

labellist newlabellist(void);
int addlabel(labellist list, char *label, char *type, int vals, int linenum);
void shiftdatalabels(labellist list, int delta);
void freelabellist(labellist list);
int setentry(labellist list, char *lbl);
int getval(labellist list, char* lbl, int address);
int AREbylabel(labellist list, char *lbl);
void createentext(labellist list, char *filename);