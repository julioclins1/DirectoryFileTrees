/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "dynarray.h"
#include "ft.h"
#include "directory.h"
#include "file.h"
#include "checkerFT.h"

/* A File Tree is an AO with 3 state variables: */

/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitialized;

/* a pointer to the root directory in the hierarchy */
static Node_T root;

/* a counter of the number of directories and files in the hierarchy */
static size_t count;

/* Starting at the parameter curr, traverses as far down
   the directory hierarchy as possible while still matching
   the path parameter.

   returns a pointer to the farthest matching directory down
   that path, or NULL if there is no directory in dir's
   hierarchy that matches a prefrix of the path 
*/
static Node_T FT_traversePath(Dir_T curr, char* path) {

   Dir_T found;
   size_t i;
   size_t lenChild;
   char* currPath;
   enum {EQUAL};

   assert(path != NULL);

   if (curr == NULL)
      return NULL;

   assert(CheckerFT_Dir_isValid(curr));

   currPath = Node_getPath(curr);
   lenChild = Dir_getNumDir(curr);

   /* if current path matches full path, return current directory */
   if (strcmp(path, currPath) == EQUAL)
      return curr;

   /* if current path is a prefix of path */
   if (strncmp(path, currPath, strlen(currPath)) == EQUAL) {

      for (i = 0; i < lenChild; i++) {
         found = FT_traversePath(path, Dir_getDir(curr, i));

         if (found != NULL)
            return found;
      }  
      return curr;
   }

   return NULL;
}

/* Inserts a new path of subdirectories into the tree rooted at parent,
   or, if parent is NULL, as the root of the data structure.

   If there is an allocation error in creating any of the new
   directories or their fields, returns MEMORY_ERROR

   If path is not underneath existing root, returns CONFLICTING_PATH

   If there is an error linking any of the new directories,
   returns PARENT_CHILD_ERROR

   Otherwise, returns SUCCESS 
*/
static int FT_insertRestOfDir(Dir_T parent, char* path) {

   Dir_T curr = parent;
   Dir_T firstNew = NULL;
   Dir_T new;
   char* copyPath;
   char* restPath = path;
   char* dirToken;
   int result;
   size_t newCount = 0;

   assert(path != NULL);

   /* If not underneath existing root */
   if (curr == NULL && root != NULL)
      return CONFLICTING_PATH;

   restPath += (strlen(Dir_getPath(curr)) + 1);

   copyPath = malloc(strlen(restPath) + 1);
   if (copyPath == NULL)
      return MEMORY_ERROR;

   strcpy(copy Path, restPath);
   dirToken = strtok(copyPath, "/");

   while (dirToken != NULL) {
      new = Dir_create(dirToken, curr);
      newCount++;

      if (firstNew == NULL)
         firstNew = new;

      else {
         result = FT_linkParentToChild(curr, new);
         if (result != SUCCESS) {
            (void) Dir_destroy(new);
            (void) Node_destroy(firstNew);
            free(copyPath);
            return result;
         }
      }

      if (new == NULL) {
         (void) Dir_destroy(firstNew);
         free(copyPath);
         return MEMORY_ERROR;
      }
      curr = new;
      dirToken = strtok(NULL, "/");
   }

   free(copyPath);

   if (parent == NULL) {
      root = firstNew;
      count = newCount;
      return SUCCESS;
   }

   result = FT_linkParentToChild(parent, firstNew);

   if (result == SUCCESS)
      count += newCount;

   else
      (void) Dir_destroy(firstNew);

   return result;
}

/* see ft.h for specification */
int FT_insertDir(char* path) {

   Dir_T dir;
   int result;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = FT_traversePath(path);

   if (dir != NULL && strcmp(path, Dir_getPath(dir)) == EQUAL)
      return ALREADY_IN_TREE;

   if (dir != NULL && Dir_hasFile(dir, path, NULL) == TRUE)
      return NOT_A_DIRECTORY;

   result = FT_insertRestOfDir(dir, path);
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* Starting at parameter dir, looks for a directory whose full path 
   matches the parameter path. Returns a pointer to such directory if 
   it is found. Otherwise, returns NULL
*/
static Dir_T FT_getDir(Dir_T dir, char* path) {

   Dir_T result;
   enum {EQUAL};

   assert(CheckerFFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   result = FT_traversePath(dir, path);

   if (result == NULL)
      return NULL;

   if (strcmp(path, Dir_getPath(dir) == EQUAL))
      return result;

   return NULL;
}

/* see ft.h for specification */
boolean FT_containsDir(char* path) {
   
   Dir_T dir;
   boolean result = FALSE;
   enum {EQUAL};

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   dir = FT_getDir(path);

   if (dir != NULL)
      result = TRUE;
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* Destroys the entire herarchy of directories and files rooted
   at parameter dir, including dir itself, updating count accordingly.
   If dir is the root, it points the root to NULL. 

   Returns SUCCESS
*/
static int FT_removeDirFrom(Dir_T dir) {

   if (dir == root)
      root = NULL
   
   if (dir != NULL)
      count -= Dir_destroy(dir);

   return SUCCESS;
}

/* see ft.h for specification */
int FT_rmDir(char *path) {

   Dir_T dir;
   int result;
   enum {EQUAL};

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = FT_traversePath(root, path);

   if (dir == NULL)
      result = NO_SUCH_PATH;

   if (strcmp(path, Dir_getPath(dir)) != EQUAL) {

      if (Dir_hasFile(dir, path, NULL) == TRUE)
         return NOT_A_DIRECTORY;

      else
         return NO_SUCH_PATH;
   }

   result = FT_removeDirFrom(dir);

   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* for a given file whose path (parameter path) is "prefix/filename," 
   splits prefix and returns it.

   If unable to allocate sufficient memory, returns NULL.

   Allocates memory for prefix, which is then owned by the caller!
*/
static char *FT_getPrefix(char *path) {

   char* prefix;
   size_t index = 0;
   size_t i = 0;
   
   assert(path != NULL);

   while (*(path[i]) != '\n') {

      if (*(path[i]) == '/')
         index = i;

      i++;
   }

   if (index == 0)
      index = i + 1;

   /* enough memory for char* from 0 to index-1 plus \0 */
   prefix = (char*)malloc(index);

   if (prefix == NULL)
      return NULL;

   *prefix = '\0';

   strncpy(prefix, path, index - 1);
   strcat(prefix, '\0');

   return prefix;
}


/* see ft.h for specification */
int FT_insertFile(char *path, void *contents, size_t length) {

   Dir_T parent;
   File_T file;
   int result;
   char* prefix;
   enum {EQUAL};


   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;
   
   parent = FT_traversePath(root, path);

   if (parent == NULL)
      return CONFLICTING_PATH;

   if (strcmp(path, Dir_getPath(parent)) == EQUAL)
      return NOT_A_FILE;

   if (Dir_hasFile(parent, path, NULL) == TRUE)
      return ALREADY_IN_TREE;

   prefix = FT_getPrefix(path);

   if (prefix == NULL)
      return MEMORY_ERROR;

   result = FT_insertRestOfDir(parent, prefix);

   if (result != SUCCESS) {
      free(prefix);
      return result;
   }

   parent = FT_traversePath(parent, prefix);

   assert(CheckerFT_Dir_isValid(parent));
   assert(strcmp(prefix, Dir_getPath(parent)) == EQUAL);
   assert(strcmp(prefix, path) != EQUAL);

   free(prefix);

   file = File_create(parent, path, contents, length);

   if (file == NULL)
      return MEMORY_ERROR;

   result = Dir_linkFile(parent, file);

   if (result != SUCCESS)
      return result;

   count++;
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(CheckerFT_File_isValid(file));

   return SUCCESS;
}

/* Starting at parameter dir, looks for file whose full path matches 
   the parameter path. Returns a pointer to such file if it is found.
   Otherwise, returns NULL
*/
static File_T FT_getFile(Dir_T dir, char* path) {

   Dir_T parent;
   size_t childID;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   parent = FT_traversePath(dir, path);

   if (parent == NULL)
      return NULL;

   if (Dir_hasFile(parent, path, &childID) == TRUE)
      return Dir_getFile(parent, childID);

   return NULL;
}

/* see ft.h for specification */
boolean FT_containsFile(char* path) {
   
   File_T file;
   boolean result = FALSE;
  
   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   file = FT_getFile(path);

   if (file != NULL)
      result = TRUE;

   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;

}

/* see ft.h for specification */
int FT_rmFile(char *path) {

   Dir_T parent;
   File_T file;
   size_t childID;
   enum {EQUAL};

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   parent = FT_traversePath(root, path);

   if (parent == NULL)
      return NO_SUCH_PATH;

   if (strcmp(path, Dir_getPath(parent)) == EQUAL)
      return NOT_A_FILE;

   if (Dir_hasFile(parent, path, &childID) != TRUE)
      return NO_SUCH_PATH; 
   
   file = Dir_getFile(parent, childID);

   (void) Dir_unlinkFile(parent, file, childID);

   File_destroy(file);

   count--;

   return SUCCESS;
}

/* see ft.h for specification */
void *FT_getFileContents(char *path) {
   File_T file;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return NULL;

   file = FT_getFile(root, path);

   if (file == NULL)
      return NULL;

   return File_getContents(file);
}

/* see ft.h for specification */
void *FT_replaceFileContents(char *path, void *newContents,
                             size_t newLength) {

   File_T file;

   assert(ChekerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return NULL;

   file = FT_getFile(root, path);

   if (file == NULL)
      return NULL;

   return File_replaceContents(file, newContents, newLength);
}

/* see ft.h for specification */
int FT_stat(char *path, boolean *type, size_t *length) {

   Dir_T dir;
   File_T file;
   size_t childID;
   enum {EQUAL};

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);
   assert(type != NULL);
   assert(length != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = FT_traversePath(root, path);

   if (strcmp(path, Dir_getPath(dir)) == EQUAL) {
      *type = FALSE;
      return SUCCESS;
   }

   if (Dir_hasFile(dir, path, &childID) == TRUE) {
      file = Dir_getFile(dir, childID);
      *type = TRUE;
      *length = File_getLength(file);
      return SUCCESS;
   }
   
   return NO_SUCH_PATH;
}

/* see ft.h for specification */
int FT_init(void) {

   assert(CheckerFT_isValid(isInitialized, root, count));

   if (isInitialized)
      return INITIALIZATION_ERROR;

   isInitialized = 1;
   root = NULL;
   count = 0;

   assert(CheckerFT_isValid(isInitialized, root, count));
   return SUCCESS;
}

/* see ft.h for specification */
int FT_destroy(void) {

   assert(CheckerFT_isValid(isInitialized, root, count));

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   FT_removeDirFrom(root);

   isInitialized = 0;

   assert(CheckerFT_isValid(isInitialized, root, count));
   return SUCCESS;
}

/* Performs a pre-order traversal of the tree rooted at parameter dir,
   inserting each payload to DynArray_T dArray beginning at index i.
   Returns the next unused index in dArray after the insertion(s).

   Maintains the invariance that files are inserted first than
   directories, when applicable.
*/
static size_t FT_preOrderTraversal(Dir_T dir, DynArray_T dArray,
                                   size_t i) {

   size_t childID;
   File_T filePath;

   assert(dArray != NULL);

   if (dir != NULL) {

      (void) DynArray_set(dArray, i, Dir_getPath(dir));
      i++;

      for (childID = 0; childID < Dir_getNumFiles(dir); childID++) {

         filePath = File_getPath(Dir_getFile(dir, childID));
         
         DynArray_set(dArray, i, filePath);

         i++;
      }

      for (childID = 0; childID < Dir_getNumDir(curr); childID++)
         i = FT_preOrderTraversal(Dir_getDir(dir, childID), dArray, i);
   }
   
   return i;
}

/* Alternate version of strlen that uses pAcc as an in-out parameter
   to accumulate a string length, rather than returning the length of
   str, and also always adds one more in addition to str's length
*/
static void FT_strlenAccumulate(char* str, size_t* pAcc) {
   
   assert(pAcc != NULL);

   if(str != NULL)
      *pAcc += (strlen(str) +1);
}

/* Alternate version of strcat that inverts the typical argument order,
   appending str onto acc, and also always adds a newline at the end of
   the concatenated string.
*/
static void FT_strcatAccumulate(char* str, char* acc) {
   
   assert(acc != NULL);

   if (str!= NULL) {
      strcat(acc, str);
      strcat(acc, '\n');
   }
}

/* see ft.h for specification */
char *FT_toString(void) {

   DynArray_T paths;
   size_T totalStrlen = 1;
   char* result = NULL;

   assert(CheckerFT_isValid(isInitialized, root, count));

   if (!isInitialized)
      return NULL;

   paths = DynArray_new(count);
   (void) FT_preOrderTraversal(root, paths, 0);

   DynArray_map(paths, (void (*)(void *, void *)) FT_strlenAccumulate,
                (void *) &totalStrLen);

   result = malloc(totalStrlen);
   if (result == NULL) {
      DynArray_free(paths);
      assert(CheckerFT_isValid(isInitialized, root, count));
      return NULL;
   }
   *result = '\0';

   DynArray_map(paths, (void (*)(void *, void*)) FT_strcatAccumulate,
                (void *) result);

   DynArray_free(paths);
   assert(CheckerFT_isValid(isInitliazed, root, count));
   return result;
}
