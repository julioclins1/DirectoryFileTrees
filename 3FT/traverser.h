/*--------------------------------------------------------------------*/
/* traverser.h                                                        */
/* Authors: Julio Lins and Rishabh Rout                               */
/*--------------------------------------------------------------------*/

#include <stddef.h>
#include "a4def.h"
#include "directory.h"
#include "file.h"

/* 
   Traverser is a stateless module whose functions are related to the
   traversal of the FT and of its paths, without creating or deleting
   any path
*/

/* Starting at the parameter curr, traverses as far down
   the directory hierarchy as possible while still matching
   the path parameter.

   returns a pointer to the farthest matching directory down
   that path, or NULL if there is no directory in dir's
   hierarchy that matches a prefrix of the path 
*/
Dir_T Traverser_traversePath(Dir_T curr, char* path);


/* Returns NOT_A_DIRECTORY if proper prefix of path exists in the tree 
   as a file. Returns MEMORY_ERROR if unable to allocate sufficient
   memory. Returns SUCCESS if there is no file with such proper prefix

   Parameter path is the full path at hand. dir is farthest matching
   directory in the path
*/
int Traverser_NotADir(Dir_T dir, const char* path);

/* 
   Starting at parameter dir, looks for a directory whose full path 
   matches the parameter path.

   Returns a pointer to such directory if 
   it is found. Otherwise, returns NULL
*/
Dir_T Traverser_getDir(Dir_T dir, char* path);


/* For a given file whose path (parameter path) is "prefix/filename," 
   creates string prefix and returns it.

   If path is unique (i.e, root's path), returns a copy of path.

   If unable to allocate sufficient memory, returns NULL.

   Allocates memory for prefix, which is then owned by the caller!
*/
char *Traverser_getPrefix(char *path);

/* 
   Starting at parameter dir, looks for file whose full path matches 
   the parameter path. Returns a pointer to such file if it is found.

   Otherwise, returns NULL
*/
File_T Traverser_getFile(Dir_T dir, char* path);


/* Returns a string representation of the data structure rooted at 
   parameter root, or NULL if there is an allocation error

   count is the number of elements (files or directories) in the
   data structure.

   Allocates memory for the returned string,
   which is then owned by the client!
*/
char *Traverser_toString(Dir_T root, size_t count);

