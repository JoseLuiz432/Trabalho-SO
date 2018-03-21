###################################MAKEFILE######################################

EXEC=vesh

all:	vesh
vesh:	bib.o	vesh.o
	gcc -o vesh bib.o vesh.o

bib.o:	bib.c
	gcc -o bib.o -c bib.c

vesh.o:	vesh.c	bib.h
	gcc -o vesh.o -c vesh.c

clean: 
	rm -rf *.o

mrproper: clean
	rm -rf vesh