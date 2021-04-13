/*--------------------------------------------------------------------*/
/* file.h                                                             */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#ifndef FILE_INCLUDED
#define FILE_INCLUDED

#include "defs.h"

#include <stddef.h>
#include "a4def.h"
#include "directory.h"



/*
   a File_T is an object that contains a path payload and references to
   the file's parent, contents, and length.
*/
/* typedef struct file* File_T; */


/*
   Given a file's parent directory, full path, and contents with
   respective length in bytes, returns a new File_T storing such 
   information or NULL if any allocation error occurs in creating 
   the file or its fields.

   The parent is not changed to link to the file.
*/
File_T File_create(Dir_T parent, const char *path, void *contents,
                   size_t length);
/*
  Frees File_T file
*/
void File_destroy(File_T file);


/*
  Compares file1 and file2 based on their paths.
  Returns <0, 0, or >0 if file1 is less than,
  equal to, or greater than file2, respectively.
*/
int File_compare(File_T file1, File_T file2);

/*
   Returns file's path.
*/
const char* File_getPath(File_T file);

/*
   Returns the parent directory of file
*/
Dir_T File_getParent(File_T file);

/* 
   Returns the contents associated with file

   Note that contents can be NULL
 */
void *File_getContents(File_T file);

/* 
   Replaces the contents of file with newContents,
   updating the length stored in file accordingly.

   Returns the old contents.

   Node that the old contents can be NULL
*/
void *File_replaceContents(File_T file, void* newContents,
                           size_t newLength);

/* 
   Returns the length in bytes associated with the
   contents in file
*/
size_t File_getLength(File_T file);

/*
  Returns a string representation for file, 
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char* File_toString(File_T file);

#endif
