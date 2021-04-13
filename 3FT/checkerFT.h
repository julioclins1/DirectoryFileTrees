/*--------------------------------------------------------------------*/
/* checkerDT.h                                                        */
/* Author: Christopher Moretti                                        */
/*--------------------------------------------------------------------*/

#ifndef CHECKER_INCLUDED
#define CHECKER_INCLUDED

#include "directory.h"
#include "file.h"


/*
   Returns TRUE if n represents a directory entry
   in a valid state, or FALSE otherwise.
*/
boolean CheckerFT_Dir_isValid(Dir_T dir);

/*
   Returns TRUE if n represents a directory entry
   in a valid state, or FALSE otherwise.
*/
boolean CheckerFT_File_isValid(File_T file);

/*
   Returns TRUE if the hierarchy is in a valid state or FALSE
   otherwise.  The data structure's validity is based on a boolean
   isInit indicating whether it has been initialized, a Node_T root
   representing the root of the hierarchy, and a size_t count
   representing the total number of directories in the hierarchy.
*/
boolean CheckerFT_isValid(boolean isInit, Dir_T root, size_t count);

#endif
