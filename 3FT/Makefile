#-----------------------------------------------------------------------
# Makefile for Assignment 4, Part 3
# Authors: Julio Lins and Rishabh Rout
#-----------------------------------------------------------------------

# Macros
CC=gcc217
# CC = gcc217m

CFLAGS =
# CFLAGS = -g
# CFLAGS = -D NDEBUG
# CFLAGS = -D NDEBUG -0

# Dependency rules for non-file targets
all: ft_client
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f ft_client *.o


# Dependency rules for file targets
ft_client: ft_client.o ft.o traverser.o file.o directory.o \
checkerFT.o dynarray.o
	$(CC) $(CFLAGS2) ft_client.o ft.o traverser.o \
file.o directory.o checkerFT.o dynarray.o -o ft_client


ft_client.o: ft_client.c ft.h a4def.h
	$(CC) $(CFLAGS) -c ft_client.c

ft.o: ft.c ft.h a4def.h traverser.h directory.h file.h checkerFT.h \
dynarray.h defs.h
	$(CC) $(CFLAGS) -c ft.c

traverser.o: traverser.c traverser.h ft.h dynarray.h checkerFT.h \
defs.h a4def.h file.h directory.h
	$(CC) $(CFLAGS) -c traverser.c

dynarray.o: dynarray.c dynarray.h
	$(CC) $(CFLAGS) -c dynarray.c

file.o: file.c file.h checkerFT.h directory.h defs.h
	$(CC) $(CFLAGS) -c file.c

directory.o: directory.c directory.h dynarray.h checkerFT.h file.h \
a4def.h defs.h
	$(CC) $(CFLAGS) -c directory.c

checkerFT.o: checkerFT.c checkerFT.h file.h directory.h defs.h a4def.h
	$(CC) $(CFLAGS) -c checkerFT.c

