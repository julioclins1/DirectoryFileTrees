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
	rm -f ft_client dynarray.o ft.o file.o directory.o checkerFT.o


# Dependency rules for file targets
ft_client: ft_client.o dynarray.o ft.o file.o directory.o checkerFT.o
	$(CC) $(CFLAGS2) ft_client.o dynarray.o ft.o file.o directory.o\
 checkerFT.o -o ft_client


ft_client.o: ft_client.c ft.h
	$(CC) $(CFLAGS) -c ft_client.c
dynarray.o: dynarray.c dynarray.h
	$(CC) $(CFLAGS) -c dynarray.c
ft.o: ft.c ft.h directory.h file.h checkerFT.h
	$(CC) $(CFLAGS) -c ft.c
file.o: file.c file.h dynarray.h checkerFT.h directory.h
	$(CC) $(CFLAGS) -c file.c
directory.o: directory.c directory.h dynarray.h checkerFT.h file.h
	$(CC) $(CFLAGS) -c directory.c
checkerFT.o: checkerFT.c checkerFT.h
	$(CC) $(CFLAGS) -c checkerFT.c

