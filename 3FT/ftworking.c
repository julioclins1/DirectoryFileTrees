/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "ft.h"
#include "defs.h"


/* A File Tree is an AO with 3 state variables: */

/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitialized;

/* a pointer to the root directory in the hierarchy */
static Dir_T root;

/* a counter of the number of directories and files in the hierarchy */
static size_t count;

/* Starting at the parameter curr, traverses as far down
   the directory hierarchy as possible while still matching
   the path parameter.

   returns a pointer to the farthest matching directory down
   that path, or NULL if there is no directory in dir's
   hierarchy that matches a prefrix of the path 
*/
static Dir_T FT_traversePath(Dir_T curr, char* path) {

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
   lenDirC = Dir_getNumDir(curr);

   /* if current path matches full path, returns current directory */
   if (strcmp(path, currPath) == EQUAL)
      return curr;

   /* if current path is a prefix of full path */
   if (strncmp(path, currPath, strlen(currPath)) == EQUAL) {

      for (i = 0; i < lenDirC; i++) {
         child = Dir_getDir(curr, i);
         found = FT_traversePath(child, path);
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
static int FT_NotADir(Dir_T dir, const char* path) {

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
   copy = (char*)malloc(index);

   if (copy == NULL)
      return MEMORY_ERROR;

   /* copy parentPath/next */
   for (i = 0; i < index; i++)
      copy[i] = path[i];

   copy[i] = '\0';
         

   if (Dir_hasFile(dir, copy, NULL) == TRUE) {
      free(copy);
      return NOT_A_DIRECTORY;
   }

   free(copy);
   return SUCCESS;
}

/*
  Given a prospective parent and child directory,
  adds child to parent's children directory list, if possible

  If not possible, destroys the hierarchy rooted at child
  and returns PARENT_CHILD_ERROR, otherwise, returns SUCCESS.
*/
static int FT_linkParentToDir(Dir_T parent, Dir_T child) {

   assert(parent != NULL);

   if (Dir_linkDir(parent, child) != SUCCESS) {
      (void) Dir_destroy(child);
      return PARENT_CHILD_ERROR;
   }
   return SUCCESS;
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

   if (curr != NULL)
      restPath += (strlen(Dir_getPath(curr)) + 1);

   copyPath = malloc(strlen(restPath) + 1);
   if (copyPath == NULL)
      return MEMORY_ERROR;

   strcpy(copyPath, restPath);
   dirToken = strtok(copyPath, "/");

   /* For each token separated by / in path,
      creates new directory  */
   while (dirToken != NULL) {
      new = Dir_create(curr, dirToken);
      newCount++;

      /* saves first dir created for future 
         linkage with parent */
      if (firstNew == NULL)
         firstNew = new;

      else {        
         result = FT_linkParentToDir(curr, new);

         /* if linkage fails, destroys previous insertion(s) and
            reports error */
         if (result != SUCCESS) {
            (void) Dir_destroy(firstNew);
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

   /* if firstNew should be the root */
   if (parent == NULL) {
      root = firstNew;
      count = newCount;
      return SUCCESS;
   }

   result = FT_linkParentToDir(parent, firstNew);

   if (result == SUCCESS)
      count += newCount;

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

   dir = FT_traversePath(root, path);

   if (dir != NULL) {

   /* Checks if path is already in tree as a directory */
   if (strcmp(path, Dir_getPath(dir)) == EQUAL)
      return ALREADY_IN_TREE;

   /* Checks if path is already in tree as a file */
   if (Dir_hasFile(dir, path, NULL) == TRUE)
      return ALREADY_IN_TREE;

   /* Checks if a proper prefix of path exists as a file */
   result = FT_NotADir(dir, path);
   if (result != SUCCESS)
      return result;
   }

   result = FT_insertRestOfDir(dir, path);
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* 
   Starting at parameter dir, looks for a directory whose full path 
   matches the parameter path. Returns a pointer to such directory if 
   it is found. Otherwise, returns NULL
*/
static Dir_T FT_getDir(Dir_T dir, char* path) {

   Dir_T result;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   result = FT_traversePath(dir, path);

   if (result == NULL)
      return NULL;

   if (strcmp(path, Dir_getPath(result)) == EQUAL)
      return result;

   return NULL;
}

/* see ft.h for specification */
boolean FT_containsDir(char* path) {
   
   Dir_T dir;
   boolean result = FALSE;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   dir = FT_getDir(root, path);

   if (dir != NULL)
      result = TRUE;
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* 
   Destroys the entire herarchy of directories and files rooted
   at parameter dir, including dir itself, updating count accordingly.
   If dir is the root, it points the root to NULL. 

   Returns SUCCESS
*/
static int FT_removeDirFrom(Dir_T dir) {

   if (dir == root)
      root = NULL;
   
   if (dir != NULL)
      count -= Dir_destroy(dir);

   return SUCCESS;
}

/* see ft.h for specification */
int FT_rmDir(char *path) {

   Dir_T dir;
   int result;
   Dir_T parent;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = FT_traversePath(root, path);

   /* Checks if path does not exist or exists as a file */
   if (dir == NULL)
      return NO_SUCH_PATH;

   if (strcmp(path, Dir_getPath(dir)) != EQUAL) {
      if (Dir_hasFile(dir, path, NULL) == TRUE)
         return NOT_A_DIRECTORY;
      
      else
         return NO_SUCH_PATH;
   }

   /* Unlink parent and child directories if dir is not the root */
   parent = Dir_getParent(dir);
   if (parent != NULL)
      Dir_unlinkDir(parent, dir);

   result = FT_removeDirFrom(dir);
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* for a given file whose path (parameter path) is "prefix/filename," 
   creates string prefix and returns it.

   If unable to allocate sufficient memory, returns NULL.

   Allocates memory for prefix, which is then owned by the caller!
*/
static char *FT_getPrefix(char *path) {

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
   prefix = (char*)malloc(index);

   if (prefix == NULL)
      return NULL;

   for (i = 0; i < index; i++)
      prefix[i] = path[i];

   prefix[index] = '\0';

   return prefix;
}

/*
  Given a prospective parent directory and child file,
  adds child to parent's children file list, if possible

  If not possible, destroys the file and returns PARENT_CHILD_ERROR, 
  otherwise, returns SUCCESS.
*/
static int FT_linkParentToFile(Dir_T parent, File_T child) {

   assert (parent != NULL);

   if(Dir_linkFile(parent, child) != SUCCESS) {
      File_destroy(child);
      return PARENT_CHILD_ERROR;
   }

   return SUCCESS;
}

/* see ft.h for specification */
int FT_insertFile(char *path, void *contents, size_t length) {

   Dir_T parent;
   File_T file;
   int result;
   char* prefix;


   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   /* Ensures that file will not be the root */
   if (root == NULL)
      return CONFLICTING_PATH;
   
   parent = FT_traversePath(root, path);

   /* Checks if path is not underneath existing root */
   if (parent == NULL)
      return CONFLICTING_PATH;

   /* Checks if path is already in tree as a directory */
   if (strcmp(path, Dir_getPath(parent)) == EQUAL)
      return ALREADY_IN_TREE;

   /* Checks if path is already in tree as a file */
   if (Dir_hasFile(parent, path, NULL) == TRUE)
      return ALREADY_IN_TREE;

   /* Checks if a proper prefix of path exists as a file */
   result = FT_NotADir(parent, path);
   if (result != SUCCESS)
      return result;


   /* Gets the path of the new file's parent directory */
   prefix = FT_getPrefix(path);
   if (prefix == NULL)
      return MEMORY_ERROR;

   /* If current parent should not be the new file's parent,
      insert rest of directories and get the file's  true parent */
   if (strcmp(prefix, Dir_getPath(parent)) != EQUAL) {     
   result = FT_insertRestOfDir(parent, prefix);
   if (result != SUCCESS) {
      free(prefix);
      return result;
   }
   parent = FT_traversePath(parent, prefix);
   }

   /* Asserts invariances */
   assert(CheckerFT_Dir_isValid(parent));
   assert(strcmp(prefix, Dir_getPath(parent)) == EQUAL);
   assert(strcmp(prefix, path) != EQUAL);

   free(prefix);

   file = File_create(parent, path, contents, length);

   if (file == NULL)
      return MEMORY_ERROR;

   if (FT_linkParentToFile(parent, file) != SUCCESS)
      return PARENT_CHILD_ERROR;

   count++;
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(CheckerFT_File_isValid(file));

   return SUCCESS;
}

/* 
   Starting at parameter dir, looks for file whose full path matches 
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

   file = FT_getFile(root, path);

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

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   parent = FT_traversePath(root, path);

   /* Checks if path does not exist or exists as a directory */
   if (parent == NULL)
      return NO_SUCH_PATH;

   if (strcmp(path, Dir_getPath(parent)) == EQUAL)
      return NOT_A_FILE;

   if (Dir_hasFile(parent, path, &childID) != TRUE)
      return NO_SUCH_PATH; 
   
   file = Dir_getFile(parent, childID);

   /* Removes file and updates count */
   (void) Dir_unlinkFile(parent, file);
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

   assert(CheckerFT_isValid(isInitialized, root, count));
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

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);
   assert(type != NULL);
   assert(length != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = FT_traversePath(root, path);

   /* Checks if path exists as a directory */
   if (strcmp(path, Dir_getPath(dir)) == EQUAL) {
      *type = FALSE;
      return SUCCESS;
   }

   /* Checks if path exists as a file */
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
   const char* filePath;

   assert(dArray != NULL);

   if (dir != NULL) {

      /* Starts with dir (pre order) */
      (void) DynArray_set(dArray, i, Dir_getPath(dir));
      i++;

      /* Then child files */
      for (childID = 0; childID < Dir_getNumFiles(dir); childID++) {
         filePath = File_getPath(Dir_getFile(dir, childID));
         DynArray_set(dArray, i, filePath);
         i++;
      }

      /* Then recur on child directories (depth first, pre order) */
      for (childID = 0; childID < Dir_getNumDir(dir); childID++)
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
      strcat(acc, "\n");
   }
}

/* see ft.h for specification */
char *FT_toString(void) {

   DynArray_T paths;
   size_t totalStrlen = 1;
   char* result = NULL;

   assert(CheckerFT_isValid(isInitialized, root, count));

   if (!isInitialized)
      return NULL;

   /* Populates DynArray paths with the full paths of each directory or
      files in the tree (depth first, pre order) */
   paths = DynArray_new(count);
   (void) FT_preOrderTraversal(root, paths, 0);

   /* 
      Calculates the accumulative length of the n elements in the tree
      (files or directories), + n (i.e., 1 additional byte per element)
   */
   DynArray_map(paths, (void (*)(void *, void *)) FT_strlenAccumulate,
                (void *) &totalStrlen);

   /* Allocates memory for such string representation of the tree */
   result = malloc(totalStrlen);
   if (result == NULL) {
      DynArray_free(paths);
      assert(CheckerFT_isValid(isInitialized, root, count));
      return NULL;
   }
   *result = '\0';

   /* concatenates the string representation of the tree, always with a
      newline at the end of each */
   DynArray_map(paths, (void (*)(void *, void*)) FT_strcatAccumulate,
                (void *) result);

   DynArray_free(paths);
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}
