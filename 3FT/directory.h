/*--------------------------------------------------------------------*/
/* directory.h                                                        */
/* Authors: Julio Lins and Rishabh Rout                               */
/*--------------------------------------------------------------------*/

#ifndef DIRECTORY_INCLUDED
#define DIRECTORY_INCLUDED

#include <stddef.h>
#include "a4def.h"

/*
   a Dir_T is an object that contains a path payload and references to
   the directory's parent (if it exists) and children, both files and
   subdirectories (if they exist).
*/
typedef struct directory* Dir_T;


/*
   Given a parent directory and a string dir, returns a new Dir_T or 
   NULL if any allocation error occurs in creating the directory or its
   fields.

   The new structure is initialized to have its path as the parent's
   path (if it exists) prefixed to the dir string parameter, separated
   by a slash. It is also initialized with its parent link as the 
   parent parameter value, but the parent itself is not changed
   to link to the new directory. The children links are initialized but
   do not point to any children.
*/

Dir_T Dir_create(Dir_T parent, const char* dir);

/*
  Destroys the entire hierarchy of directories and files rooted at dir,
  including dir itself.

  Returns the number of directories and files destroyed.
*/
size_t Dir_destroy(Dir_T dir);


/*
  Compares dir1 and dir2 based on their paths.
  Returns <0, 0, or >0 if dir1 is less than,
  equal to, or greater than dir2, respectively.
*/
int Dir_compare(Dir_T dir1, Dir_T dir2);

/*
   Returns dir's path.
*/
const char* Dir_getPath(Dir_T dir);

/*
  Returns the number of children of  parent dir if
  type is neither 0 (DIR) nor 1 (FILES).

  If type is 0 (DIR), returns number of child directories.
  If type is 1 (FILES), returns number of child files.
*/
size_t Dir_getNumChildren(Dir_T dir, int type);


/* If type is 0 (DIR), returns 1 if parent has a child directory
   whose full path is path, 0 if it does not have such a child,
   and -1 if there is an allocation error during search.

   If type is 1 (FILES), it works analogously for a child file

   If type is neither 0 nor 1, it works analogously, but checking if
   there is either a child directory or a child file with path

   Furthermore, if parent does have such a child, and childID is not
   NULL, it stores the child's indentifier in *childID. If parent
   does not have such a child, store the identifier that such a child
   would have in *childID.

 */
int Dir_hasChild(Dir_T parent, const char* path, size_t* childID,
                 int type);


/*
  If type is 0 (DIR), returns the Dir_T child directory of parent
  with identifier childID, if it exists.

  If type is 1 (FILES), returns the File_T child file of parent
  with identifier childID, if it exists.

  If the respective child does not exist, returns NULL.
*/
void* Dir_getChild(Dir_T parent, size_t childID, int type);

/*
   Returns the parent directory of dir, if it exists, otherwise 
   returns NULL
*/
Dir_T Dir_getParent(Dir_T dir);

/* 
   If type is 0 (DIR), makes child a child directory of parent, if
   possible, and returns SUCCESS.
   If type is 1 (FILES), makes child a child file of parent, if
   possible, and returns SUCCESS.

   If unable to link child and parent, returns PARENT_CHILD_ERROR.

   If parent already has a child with child's path, returns
   ALREADY_IN_TREE
*/
int Dir_linkChild(Dir_T parent, void* child, int type);

/*
  If type is 0 (DIR), unlinks parent from its child directory
  If type is 1 (FILES), unlinks parent from its child file
  In both cases, child is unchanged.

  Returns PARENT_CHILD_ERROR if child is not a child of parent
  and SUCCESS otherwise.
 */
int Dir_unlinkChild(Dir_T parent, void* child, int type);

/*
  Returns a string representation for dir, 
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char* Dir_toString(Dir_T dir);

#endif
