/*--------------------------------------------------------------------*/
/* directory.c                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "directory.h"
#include "file.h"
#include "checkerFT.h"

/*
   A directory structure represents a directory in the file tree
*/
struct directory {
   /* the full path of this directory */
   char* path;

   /* the parent directory of this directory
      NULL for the root of the directory tree */
   Dir_T parent;

   /* the files of this directory
      stored in sorted order by pathname */
   DynArray_T fileChildren;

   /* the subdirectories of this directory
      stored in sorted order by pathname */
   DynArray_T dirChildren;
};
