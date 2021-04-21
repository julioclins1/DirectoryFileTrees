/*--------------------------------------------------------------------*/
/* traverser.c                                                        */
/* Authors: Julio Lins and Rishabh Rout                               */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "ft.h"
#include "defs.h"
#include "traverser.h"
#include "checkerFT.h"

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
Dir_T Traverser_traversePath(Dir_T curr, char* path) {

   Dir_T found;
   Dir_T child;
   size_t i;
   size_t lenDirC;
   const char* currPath;

   if (curr == NULL)
      return NULL;

   assert(path != NULL);
   assert(CheckerFT_Dir_isValid(curr));

   currPath = Dir_getPath(curr);
   lenDirC = Dir_getNumChildren(curr, DIR);

   /* if current path matches full path, returns current directory */
   if (strcmp(path, currPath) == EQUAL)
      return curr;

   /* if current path is a prefix of full path */
   if (strncmp(path, currPath, strlen(currPath)) == EQUAL) {

      for (i = 0; i < lenDirC; i++) {
         child = Dir_getChild(curr, i, DIR);
         found = Traverser_traversePath(child, path);
         if (found != NULL)
            return found;
      }  
      return curr;
   }

   return NULL;
}

/* Returns NOT_A_DIRECTORY if proper prefix of path exists in the tree 
   as a file. Returns MEMORY_ERROR if unable to allocate sufficient
   memory. Returns SUCCESS if there is no file with such proper prefix

   Parameter path is the full path at hand. dir is farthest matching
   directory in the path
*/
int Traverser_NotADir(Dir_T dir, const char* path) {

   size_t index;
   size_t slashCount = 0;
   size_t i;
   char* copy;
   enum {SECOND_SLASH = 2};

   assert(dir != NULL);
   assert(path != NULL);

   index = strlen(Dir_getPath(dir));

   /* 
      For a given parentPath/next/restOfPath, we need to
      break not in the first, but in the second occurrence
      of / to get parentPath/next
   */
   while (path[index] != '\0') {

      if (path[index] == '/') {
         slashCount++;

            if (slashCount == SECOND_SLASH)
            break;
      }
      index++;
   }

   /* enough memory for parentPath/next + '\0' */
   copy = (char*)malloc(index+1);

   if (copy == NULL)
      return MEMORY_ERROR;

   *copy = '\0';

   /* copy parentPath/next */
   for (i = 0; i < index; i++)
      copy[i] = path[i];

   copy[i] = '\0';
         
   if (Dir_hasChild(dir, copy, NULL, FILES) == TRUE) {
      free(copy);
      return NOT_A_DIRECTORY;
   }

   free(copy);
   return SUCCESS;
}

/* 
   Starting at parameter dir, looks for a directory whose full path 
   matches the parameter path. Returns a pointer to such directory if 
   it is found. Otherwise, returns NULL
*/
Dir_T Traverser_getDir(Dir_T dir, char* path) {

   Dir_T result;

   assert(path != NULL);
   
   if (dir == NULL)
      return NULL;

   result = Traverser_traversePath(dir, path);

   if (result == NULL)
      return NULL;

   if (strcmp(path, Dir_getPath(result)) == EQUAL)
      return result;

   return NULL;
}

/* for a given file whose path (parameter path) is "prefix/filename," 
   creates string prefix and returns it.

   If unable to allocate sufficient memory, returns NULL.

   Allocates memory for prefix, which is then owned by the caller!
*/
char *Traverser_getPrefix(char *path) {

   char* prefix;
   size_t index = 0;
   size_t i = 0;
   
   assert(path != NULL);

   while (path[i] != '\0') {

      if (path[i] == '/')
         index = i;

      i++;
   }

   /* if path is the root (no /) */
   if (index == 0)
      index = i + 1;

   /* enough memory for char* from 0 to index-1 plus \0 */
   prefix = (char*)malloc(index+1);

   if (prefix == NULL)
      return NULL;

   *prefix = '\0';

   for (i = 0; i < index; i++)
      prefix[i] = path[i];

   prefix[index] = '\0';

   return prefix;
}

/* 
   Starting at parameter dir, looks for file whose full path matches 
   the parameter path. Returns a pointer to such file if it is found.
   Otherwise, returns NULL
*/
File_T Traverser_getFile(Dir_T dir, char* path) {

   Dir_T parent;
   size_t childID = 0;

   assert(dir != NULL);
   assert(path != NULL);

   parent = Traverser_traversePath(dir, path);

   if (parent == NULL)
      return NULL;

   if (Dir_hasChild(parent, path, &childID, FILES) == TRUE)
      return Dir_getChild(parent, childID, FILES);

   return NULL;
}

/* Performs a pre-order traversal of the tree rooted at parameter dir,
   inserting each payload to DynArray_T dArray beginning at index i.
   Returns the next unused index in dArray after the insertion(s).

   Maintains the invariance that files are inserted first than
   directories, when applicable.
*/
static size_t Traverser_preOrderTraversal(Dir_T dir, DynArray_T dArray,
                                   size_t i) {

   size_t childID = 0;
   Dir_T childDir;
   File_T childFile;
   const char* filePath;
   size_t numDirC;
   size_t numFileC;

   assert(dArray != NULL);

   if (dir != NULL) {

      /* Starts with dir (pre order) */
      (void) DynArray_set(dArray, i, Dir_getPath(dir));
      i++;

      /* Then child files */
      numFileC = Dir_getNumChildren(dir, FILES);
      for (childID = 0; childID < numFileC; childID++) {
         
         childFile = (File_T)Dir_getChild(dir, childID, FILES);
         filePath = File_getPath(childFile);
         (void) DynArray_set(dArray, i, filePath);
         i++;
      }

      /* Then recur on child directories (depth first, pre order) */
      numDirC = Dir_getNumChildren(dir, DIR);
      for (childID = 0; childID < numDirC; childID++) {
         
         childDir = Dir_getChild(dir, childID, DIR);
         i = Traverser_preOrderTraversal(childDir, dArray, i);
      }
   }
   return i;
}

/* Alternate version of strlen that uses pAcc as an in-out parameter
   to accumulate a string length, rather than returning the length of
   str, and also always adds one more in addition to str's length
*/
static void Traverser_strlenAccumulate(char* str, size_t* pAcc) {
   
   assert(pAcc != NULL);

   if(str != NULL)
      *pAcc += (strlen(str) +1);
}

/* Alternate version of strcat that inverts the typical argument order,
   appending str onto acc, and also always adds a newline at the end of
   the concatenated string.
*/
static void Traverser_strcatAccumulate(char* str, char* acc) {
   
   assert(acc != NULL);

   if (str!= NULL) {
      strcat(acc, str);
      strcat(acc, "\n");
   }
}

/* see ft.h for specification */
char *Traverser_toString(Dir_T root, size_t count) {

   DynArray_T paths;
   size_t totalStrlen = 1;
   char* result = NULL;
   

   /* Populates DynArray paths with the full paths of each directory or
      files in the tree (depth first, pre order) */
   paths = DynArray_new(count);
   (void) Traverser_preOrderTraversal(root, paths, 0);

   /* 
      Calculates the accumulative length of the n elements in the tree
      (files or directories), + n (i.e., 1 additional byte per element)
   */
   DynArray_map(paths, (void (*)(void *, void *))
                Traverser_strlenAccumulate, (void *) &totalStrlen);

   /* Allocates memory for such string representation of the tree */
   result = malloc(totalStrlen);
   if (result == NULL) {
      DynArray_free(paths);
      return NULL;
   }
   *result = '\0';

   /* concatenates the string representation of the tree, always with a
      newline at the end of each */
   DynArray_map(paths, (void (*)(void *, void*))
                Traverser_strcatAccumulate, (void *) result);

   DynArray_free(paths);
   return result;
}
