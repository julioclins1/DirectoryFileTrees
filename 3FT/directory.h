/*--------------------------------------------------------------------*/
/* directory.h                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#ifndef DIRECTORY_INCLUDED
#define DIRECTORY_INCLUDED

#include <stddef.h>
#include "a4def.h"
#include "file.h"

/*
   a Dir_T is an object that contains a path payload and references to
   the directories's parent (if it exists) and children, both files and
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
int Dir_compare(Node_T dir1, Node_T dir2);

/*
   Returns dir's path.
*/
const char* Dir_getPath(Dir_T n);

/*
  Returns the number of child directories dir has.
*/
size_t Dir_getNumDir(Dir_T dir);

/*
  Returns the number of child files dir has.
*/
size_t Dir_getNumFiles(Dir_T dir);

/*
   Returns the child node of n with identifier childID, if one exists,
   otherwise returns NULL.
*/
Dir_T Node_getChild(Node_T n, size_t childID);

/*
   Returns the parent directory of dir, if it exists, otherwise 
   returns NULL
*/
Dir_T Dir_getParent(Dir_T dir);

/*
  Makes child a child of parent, if possible, and returns SUCCESS.
  This is not possible in the following cases:
  * child's path is not parent's path + / + directory,
    in which case returns PARENT_CHILD_ERROR
  * parent already has a child with child's path,
    in which case returns ALREADY_IN_TREE
  * parent is unable to allocate memory to store new child link,
    in which case returns MEMORY_ERROR
 */
int Node_linkChild(Node_T parent, Node_T child);

/*
  Unlinks node parent from its child node child. child is unchanged.

  Returns PARENT_CHILD_ERROR if child is not a child of parent,
  and SUCCESS otherwise.
 */
int Node_unlinkChild(Node_T parent, Node_T child);


/*
  Returns a string representation for dir, 
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char* Dir_toString(Node_T dir);

#endif
