/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Authors: Julio Lins and Rishabh Rout                               */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "ft.h"
#include "defs.h"
#include "directory.h"
#include "file.h"
#include "checkerFT.h"
#include "traverser.h"

/* A File Tree is an AO with 3 state variables: */

/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitialized;
/* a pointer to the root directory in the hierarchy */
static Dir_T root;
/* a counter of the number of directories and files in the hierarchy */
static size_t count;


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

   *copyPath = '\0';

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

         /* if linkage fails, destroys previous insertion(s) and
            reports error */
         if (Dir_linkChild(curr, new, DIR) != SUCCESS) {
            (void) Dir_destroy(new);
            (void) Dir_destroy(firstNew);
            free(copyPath);
            return PARENT_CHILD_ERROR;
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
   
/* if linkage fails, destroys previous insertions(s) and 
   reports error */
   if (Dir_linkChild(parent, firstNew, DIR) != SUCCESS) {
      (void) Dir_destroy(firstNew);
      return PARENT_CHILD_ERROR;
   }
 
      count += newCount;

   return SUCCESS;
}

/* see ft.h for specification */
int FT_insertDir(char* path) {
   Dir_T dir;
   int result;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = Traverser_traversePath(root, path);

   if (dir != NULL) {

   /* Checks if path is already in tree as a directory */
   if (strcmp(path, Dir_getPath(dir)) == EQUAL)
      return ALREADY_IN_TREE;

   /* Checks if path is already in tree as a file */
   if (Dir_hasChild(dir, path, NULL, FILES) == TRUE)
      return ALREADY_IN_TREE;

   /* Checks if a proper prefix of path exists as a file */
   result = Traverser_NotADir(dir, path);
   if (result != SUCCESS)
      return result;
   }

   result = FT_insertRestOfDir(dir, path);
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* see ft.h for specification */
boolean FT_containsDir(char* path) {  
   Dir_T dir;
   boolean result = FALSE;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   dir = Traverser_getDir(root, path);

   if (dir != NULL)
      result = TRUE;
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
}

/* 
   Destroys the entire herarchy of directories and files rooted
   at parameter dir, including dir itself, updating count accordingly.
   If dir is the root, it points the root to NULL. Returns SUCCESS
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

   dir = Traverser_traversePath(root, path);

   /* Checks if path does not exist or exists as a file */
   if (dir == NULL)
      return NO_SUCH_PATH;

   if (strcmp(path, Dir_getPath(dir)) != EQUAL) {
      if (Dir_hasChild(dir, path, NULL, FILES) == TRUE)
         return NOT_A_DIRECTORY;
      
      else
         return NO_SUCH_PATH;
   }

   /* Unlink parent and child directories if dir is not the root */
   parent = Dir_getParent(dir);
   if (parent != NULL)
      Dir_unlinkChild(parent, dir, DIR);

   result = FT_removeDirFrom(dir);
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;
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
   
   parent = Traverser_traversePath(root, path);

   /* Checks if path is not underneath existing root */
   if (parent == NULL)
      return CONFLICTING_PATH;

   /* Checks if path is already in tree as a directory */
   if (strcmp(path, Dir_getPath(parent)) == EQUAL)
      return ALREADY_IN_TREE;

   /* Checks if path is already in tree as a file */
   if (Dir_hasChild(parent, path, NULL, FILES) == TRUE)
      return ALREADY_IN_TREE;

   /* Checks if a proper prefix of path exists as a file */
   result = Traverser_NotADir(parent, path);
   if (result != SUCCESS)
      return result;

   /* Gets the path of the new file's parent directory */
   prefix = Traverser_getPrefix(path);
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
   parent = Traverser_traversePath(parent, prefix);
   }

   /* Asserts invariances */
   assert(CheckerFT_Dir_isValid(parent));
   assert(strcmp(prefix, Dir_getPath(parent)) == EQUAL);
   assert(strcmp(prefix, path) != EQUAL);

   free(prefix);

   file = File_create(parent, path, contents, length);
   if (file == NULL)
      return MEMORY_ERROR;

   /* if linkage fails, destroys file and reports error */
   if (Dir_linkChild(parent, file, FILES) != SUCCESS) {
      File_destroy(file);
      return PARENT_CHILD_ERROR;
   }
   
   count++;
   
   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(CheckerFT_File_isValid(file));

   return SUCCESS;
}

/* see ft.h for specification */
boolean FT_containsFile(char* path) {   
   File_T file;
   boolean result = FALSE;
  
   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   file = Traverser_getFile(root, path);

   if (file != NULL)
      result = TRUE;

   assert(CheckerFT_isValid(isInitialized, root, count));
   return result;

}

/* see ft.h for specification */
int FT_rmFile(char *path) {
   Dir_T parent;
   File_T file;
   size_t childID = 0;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   parent = Traverser_traversePath(root, path);

   /* Checks if path does not exist or exists as a directory */
   if (parent == NULL)
      return NO_SUCH_PATH;

   if (strcmp(path, Dir_getPath(parent)) == EQUAL)
      return NOT_A_FILE;

   if (Dir_hasChild(parent, path, &childID, FILES) != TRUE)
      return NO_SUCH_PATH; 
   
   file = Dir_getChild(parent, childID, FILES);

   /* Removes file and updates count */
   (void) Dir_unlinkChild(parent, file, FILES);
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

   file = Traverser_getFile(root, path);

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

   file = Traverser_getFile(root, path);

   if (file == NULL)
      return NULL;

   return File_replaceContents(file, newContents, newLength);
}

/* see ft.h for specification */
int FT_stat(char *path, boolean *type, size_t *length) {
   Dir_T dir;
   File_T file;
   size_t childID = 0;

   assert(CheckerFT_isValid(isInitialized, root, count));
   assert(path != NULL);
   assert(type != NULL);
   assert(length != NULL);

   if (!isInitialized)
      return INITIALIZATION_ERROR;

   dir = Traverser_traversePath(root, path);

   /* Checks if path exists as a directory */
   if (strcmp(path, Dir_getPath(dir)) == EQUAL) {
      *type = FALSE;
      return SUCCESS;
   }

   /* Checks if path exists as a file */
   if (Dir_hasChild(dir, path, &childID, FILES) == TRUE) {
      file = Dir_getChild(dir, childID, FILES);
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
 
/* see ft.h for specification */
char *FT_toString(void) {
   assert(CheckerFT_isValid(isInitialized, root, count));

   if (!isInitialized)
      return NULL;

   return Traverser_toString(root, count);
}
