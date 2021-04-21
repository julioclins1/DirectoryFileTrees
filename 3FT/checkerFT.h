/*--------------------------------------------------------------------*/
/* checkerFT.h                                                        */
/* Authors: Julio Lins and Rishabh Rout                               */
/*--------------------------------------------------------------------*/

#ifndef CHECKER_INCLUDED
#define CHECKER_INCLUDED

#include "file.h"
#include "directory.h"

/*
   Returns TRUE if dir represents a directory entry
   in a valid state, or FALSE otherwise.
*/
boolean CheckerFT_Dir_isValid(Dir_T dir);

/*
   Returns TRUE if file represents a file entry
   in a valid state, or FALSE otherwise.
*/
boolean CheckerFT_File_isValid(File_T file);

/*
   Returns TRUE if the hierarchy is in a valid state or FALSE
   otherwise. The data structure's validity is based on a boolean
   isInit indicating whether it has been initialized, a Dir_T root
   representing the root of the hierarchy, and a size_t count
   representing the total number of directories and files in the 
   hierarchy.
*/
boolean CheckerFT_isValid(boolean isInit, Dir_T root, size_t count);

#endif
