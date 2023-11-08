void  errcommasaperation(int line);
void  errilegalLabel(int line, char* lbl);
void  errunknowencmd(int line, char *cmd);
void  wrnredundantlabel(int line);
void  errnotanumber(int line, char *str);
void  errlabelalreadyexist(int line, char *lbl);
void  errilegalstring(int line);
void errlblnfound(char *lbl, int line);
void  errilegalreg(int line);
void errextratxt(int line);
void errmcroalrdyxsist(int line, char* name);
void errlinetoolong(int line);
void errilegalvarnum(int line, int supposetobe, int exists);
void errwrongtype(int line, int varnum);
void errilrgalchar(int line);
void errfilenotexist(char *file);


