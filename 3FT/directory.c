/*--------------------------------------------------------------------*/
/* directory.c                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "directory.h"
#include "file.h"
#include "checkerFT.h"

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


/* returns a path with contents dirParent->path/dir
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

   assert(parent == NULL || CheckerFT_Dir_is_Valid(parent));
   assert(dir != NULL);

   new_dir = (Dir_T)malloc(sizeof(struct directory));
   
   if (new_dir == NULL) {
      assert(dirParent == NULL || CheckerFT_Dir_isValid(dirParent));
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

   
   assert(CheckerFT_Dir_isValid(dir));

   uDirLen = DynArray_getLength(d->dirC);
   uFileLen = DynArray_getLength(d->fileC);


   for (i = 0; i < uDirLen; i++) {

      dirChild = DynArray_get(d->dirChild, i);
      count += Dir_destroy(dirChild);
   }

   for (i = 0; i < uFileLen; i++) {

      fileChild = DynArray_get(d->fileChild, i);
      File_destroy(fileChild);
      count++;
   }

   DynArray_free(d->daDirChildren);
   DynArray_free(d->daFileChildren);

   free(d->pcPath);
   free(d);
   count++;

   return count;
}

/* see directory.h for specification */
int Dir_compare(Dir_T dir1, Dir_T dir2) {

   assert(CheckerFT_Dir_isValid(dir1));
   assert(CheckerFT_Dir_isValid(dir2));

   return strcmp(dir1->path, dir2->path);
}

/* see directory.h for specification */
const char* Dir_getPath(Dir_T dir) {

   assert(CheckerFT_Dir_isValid(dir));

   return dir->path;
}

/* see directory.h for specification */
size_t Dir_getNumDir(Dir_T dir) {

   assert(CheckerFT_Dir_isValid(dir));

   return DynArray_getLength(dir->dirC);
}


/* see directory.h for specification */
size_t Dir_getNumFiles(Dir_T dir) {

   assert(CheckerFT_Dir_isValid(dir));

   return DynArray_getLength(dir->fileC);
}

/* see directory.h for specification */
int Dir_hasDir(Dir_T parent, const char* path, size_t* childID) {

   size_t index;
   int result;
   Dir_T checker;

   assert(CheckerFT_Dir_isValid(parent));
   assert(path != NULL);

   checker = Dir_create(path, NULL);
   if(checker == NULL)
      return -1;

   result = DynArray_bsearch(parent->dirC, checker, &index,
                             (int (*)(const void*, const void*))
                             Dir_compare);

   (void) Dir_destroy(checker);

   if (childID != NULL)
      *childID = index;

   return result;
}

/* see directory.h for specification */
int Dir_hasFile(Dir_T parent, const char* path, size_t* childID) {

   size_t index;
   int result;
   Dir_T checkerParent;
   File_T checker;

   assert(CheckerFT_Dir_isValid(parent));
   assert(path != NULL);

   checkerDir = Dir_create(path, NULL);
   if (checkerDir == NULL)
      return -1;

   checker = File_create(checkerDir, path, NULL, 0);
   if (checker == NULL) {
      free(checkerDir)
      return -1;
   }

   result = DynArray_bsearch(parent->fileC, checker, &index,
                             (int (*)(const void*, const void*))
                             File_compare);

   (void) Dir_destroy(checkerDir);
   (void) File_destroy(checker);

   if (childID != NULL)
      *childID = index;

   return result;
}

/* see directory.h for specification */
Dir_T Dir_getDir(Dir_T dir, size_T childID) {

   assert(CheckerFT_Dir_isValid(dir));

   if (DynArray_getLength(dir->dirC) > childID)
      return DynArray_get(dir->dirC, childID);

   return NULL;
}

/* see directory.h for specification */
File_T Dir_getFile(Dir_T dir, size_T childID) {

   assert(CheckerFT_Dir_isValid(dir));

   if (DynArray_getLength(dir->fileC) > childID)
      return DynArray_get(dir->fileC, chilID);

   return NULL;
}

/* see directory.h for specification */
Dir_T Dir_getParent(Dir_T dir) {

   assert(CheckerFT_Dir_isValid(dir));

   return dir->parent;
}




         
