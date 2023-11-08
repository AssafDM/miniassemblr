assembler:  errors.o util.o  datalist.o firststage.o main.o secondstage.o preassembler.o  macro.o
	gcc -g -ansi  -Wall -pedantic errors.o util.o  datalist.o firststage.o main.o secondstage.o preassembler.o  macro.o -o assembler
	make clean

main.o: main.c util.o preassembler.h firststage.h secondstage.h datalist.h errors.h
	gcc -g -c -ansi  -Wall -pedantic  main.c -o main.o  

secondstage.o: secondstage.c secondstage.h errors.h datalist.h
	gcc -g -c -ansi  -Wall -pedantic  secondstage.c -o secondstage.o  


firststage.So: firststage.c firststage.h errors.h datalist.h util.h
	gcc -g -c -ansi  -Wall -pedantic  firststage.c -o firststage.o  

datalist.o: datalist.c datalist.h errors.h
	gcc -g -c -ansi  -Wall -pedantic  datalist.c -o datalist.o


preassembler.o: preassembler.c preassembler.h macro.h
	gcc -g -c -ansi  -Wall -pedantic  preassembler.c -o preassembler.o

macro.o: macro.c macro.h errors.h
	gcc -g -c -ansi  -Wall -pedantic  macro.c -o macro.o 


util.o: util.c util.h errors.h
	gcc -g -c -ansi  -Wall -pedantic  util.c -o util.o 


errors.o: errors.c errors.h
	gcc -g -c -ansi  -Wall -pedantic  errors.c -o errors.o




clean:
	rm -rf *.o

