/*--------------------------------------------------------------------*/
/* directory.c                                                        */
/* Authors: Julio Lins and Rishabh Rout                               */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "checkerFT.h"
#include "defs.h"
#include "a4def.h"

/*
   A directory structure represents a directory in the file tree
*/

struct directory {
   /* the full path of this directory */
   char* path;

   /* the parent directory of this directory
      NULL for the root of the directory tree */
   Dir_T parent;

   /* the files of this directory
      stored in sorted order by pathname */
   DynArray_T fileC;

   /* the subdirectories of this directory
      stored in sorted order by pathname */
   DynArray_T dirC;
};


/* returns a path with contents parent->path / dir
   or NULL if there is an allocation error.

   Allocates memory for the returned string,
   which is then owned by the called! */

static char* Dir_buildPath(Dir_T parent, const char* dir) {

   char* path;

   assert(dir != NULL);

   if (parent == NULL)
      path = (char*)malloc(strlen(dir) + 1);  
   else
      path = (char*)malloc(strlen(parent->path) + 1 + strlen(dir) + 1);
   
   if (path == NULL)
      return NULL;

   *path = '\0';

   if (parent != NULL) {
      strcpy(path, parent->path);
      strcat(path, "/");
   }

   strcat(path, dir);

   return path;
}

/* see directory.h for specification */
Dir_T Dir_create(Dir_T parent, const char* dir) {

   Dir_T new_dir;

   assert(parent == NULL || CheckerFT_Dir_isValid(parent));
   assert(dir != NULL);

   new_dir = (Dir_T)malloc(sizeof(struct directory));
   
   if (new_dir == NULL) {
      assert(parent == NULL || CheckerFT_Dir_isValid(parent));
      return NULL;
   }

   new_dir->path = Dir_buildPath(parent, dir);

   if (new_dir->path == NULL) {
      free(new_dir);
      assert(parent == NULL || CheckerFT_Dir_isValid(parent));
      return NULL;
   }

   new_dir->parent = parent;
   new_dir->fileC = DynArray_new(0);
   
   if (new_dir->fileC == NULL) {
      
      free(new_dir->path);
      free(new_dir);
      
      assert(parent == NULL || CheckerFT_Dir_isValid(parent));
      return NULL;
   }

   new_dir->dirC = DynArray_new(0);

   if (new_dir->dirC == NULL) {
      
      free(new_dir->fileC);
      free(new_dir->path);
      free(new_dir);

      assert(parent == NULL || CheckerFT_Dir_isValid(parent));
      return NULL;
   }

   assert(parent == NULL || CheckerFT_Dir_isValid(parent));
   assert(CheckerFT_Dir_isValid(new_dir));
   return new_dir;

}

/* see directory.h for specification */
size_t Dir_destroy(Dir_T dir) {
   
   size_t i;
   size_t count = 0;
   
   Dir_T dirChild;
   size_t uDirLen;
   
   File_T fileChild;
   size_t uFileLen;

   
   assert(dir != NULL);

   uDirLen = DynArray_getLength(dir->dirC);
   uFileLen = DynArray_getLength(dir->fileC);


   for (i = 0; i < uDirLen; i++) {

      dirChild = DynArray_get(dir->dirC, i);
      count += Dir_destroy(dirChild);
   }

   for (i = 0; i < uFileLen; i++) {

      fileChild = DynArray_get(dir->fileC, i);
      File_destroy(fileChild);
      count++;
   }

   DynArray_free(dir->dirC);
   DynArray_free(dir->fileC);

   free(dir->path);
   free(dir);
   count++;

   return count;
}

/* see directory.h for specification */
int Dir_compare(Dir_T dir1, Dir_T dir2) {

   assert(dir1 != NULL);
   assert(dir2 != NULL);

   return strcmp(dir1->path, dir2->path);
}

/* see directory.h for specification */
const char* Dir_getPath(Dir_T dir) {

   assert(dir != NULL);
   
   return dir->path;
}

/* see directory.h for specification */
size_t Dir_getNumChildren(Dir_T dir, int type) {

   size_t both;

   assert(dir != NULL);

   if (type == DIR)
      return DynArray_getLength(dir->dirC);

   if (type == FILES)
      return DynArray_getLength(dir->fileC);

   both = DynArray_getLength(dir->dirC);
   both += DynArray_getLength(dir->fileC);

   return both;
}

/* see directory.h for specification */
int Dir_hasChild(Dir_T parent, const char* path, size_t* childID,
                 int type) {
   
   size_t indexDir = 0;
   size_t indexFile = 0;
   int resultDir;
   int resultFile;
   Dir_T checkerDir;
   File_T checkerFile;

   assert(parent != NULL);
   assert(path != NULL);

   /* Creates checkers for both directories and files */
   checkerDir = Dir_create(NULL, path);
   if (checkerDir == NULL)
      return -1;

   checkerFile = File_create(checkerDir, path, NULL, 0);
   if (checkerFile == NULL) {
      free(checkerDir);
      return -1;
   }

   resultDir = DynArray_bsearch(parent->dirC, checkerDir, &indexDir,
                                (int (*)(const void*, const void*))
                                Dir_compare);

   resultFile = DynArray_bsearch(parent->fileC, checkerFile, &indexFile,
                                 (int (*)(const void*, const void*))
                                 File_compare);

   (void) Dir_destroy(checkerDir);
   File_destroy(checkerFile);

   
   /* 
      if type was defined, return respective result.
      If childID is not NULL, assigns respective childID
   */
   if (type == DIR) {
      
      if (childID != NULL)
         *childID = indexDir;

      return resultDir;
   }
 
   if (type == FILES) {

      if(childID != NULL)
         *childID = indexFile;

      return resultFile;
   }

   
   /* 
      If type is not specified, returns 1 if there is such a child
      (either file or directory), or 0 otherwise. childID is unchanged

   */

   if (resultDir == 1 || resultFile == 1)
      return TRUE;
   
   return FALSE;
}

/* see directory.h for specification */
void* Dir_getChild(Dir_T parent, size_t childID, int type) {
   assert(parent != NULL);
   assert(type == DIR || type == FILES);

   if (type == DIR && DynArray_getLength(parent->dirC) > childID)
      return DynArray_get(parent->dirC, childID);

   if (type == FILES && DynArray_getLength(parent->fileC) > childID)
      return DynArray_get(parent->fileC, childID);

   return NULL;
}

/* see directory.h for specification */
Dir_T Dir_getParent(Dir_T dir) {
   assert (dir != NULL);
   return dir->parent;
}

/* see directory.h for specification */
int Dir_linkChild(Dir_T parent, void* child, int type) {

   size_t i;
   const char* rest;
   const char* childPath;
   DynArray_T children;

   assert(type == DIR || type == FILES);
   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerFT_Dir_isValid(parent));

   if (type == DIR) {
      childPath = Dir_getPath((Dir_T)child);
      children = parent->dirC;
   }

   else {
      childPath = File_getPath((File_T)child);
      children = parent->fileC;
   }

   /* If parent's path is not a prefix of child's path */
   i = strlen(parent->path);
   if(strncmp(childPath, parent->path, i) != EQUAL)
      return PARENT_CHILD_ERROR;

   /* if parent's path and rest of child's path are not
      separated by '/' */
   rest = childPath + i;
   if (strlen(childPath) >= i && rest[0] != '/')
      return PARENT_CHILD_ERROR;

   /* check that there are no more subdirectories */
   rest++;
   if (strstr(rest, "/") != NULL)
      return PARENT_CHILD_ERROR;

 if (DynArray_bsearch(children, child, &i,
                        (int (*)(const void*, const void*))
                        File_compare) == 1) {
      return ALREADY_IN_TREE;
   }

   if (DynArray_addAt(children, i, child) == TRUE) {

      if (type == DIR)
         assert(CheckerFT_Dir_isValid((Dir_T)child));

      else
         assert(CheckerFT_File_isValid((File_T)child));
      
      return SUCCESS;
   }

   return PARENT_CHILD_ERROR;
}

/* see directory.h for specification */
int Dir_unlinkChild(Dir_T parent, void* child, int type) {

   DynArray_T children;
   size_t childID = 0;

   assert(child != NULL);

   if (type != DIR && type != FILES)
      return PARENT_CHILD_ERROR;
   
   if (type == DIR)
      children = parent->dirC;

   else 
      children = parent->fileC;
   

   /* Finds child and stores its childID */
   if(DynArray_bsearch(children, child, &childID,
                       (int (*)(const void*, const void*))
                       Dir_compare) == 0) {

      assert(CheckerFT_Dir_isValid(parent));
      return PARENT_CHILD_ERROR;
   }

   (void) DynArray_removeAt(children, childID);

   assert(CheckerFT_Dir_isValid(parent));
   return SUCCESS;
}
         
/* see directory.h for specification */
char* Dir_toString(Dir_T dir) {
   
   char* copyPath;

   assert(dir != NULL);

   copyPath = malloc(strlen(dir->path)+1);
   if(copyPath == NULL)
      return NULL;

   *copyPath = '\0';

   return strcpy(copyPath, dir->path);
}
