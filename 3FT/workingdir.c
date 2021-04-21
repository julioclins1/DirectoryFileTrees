/*--------------------------------------------------------------------*/
/* directory.c                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "defs.h"

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
size_t Dir_getNumDir(Dir_T dir) {

   assert(dir != NULL);

   return DynArray_getLength(dir->dirC);
}


/* see directory.h for specification */
size_t Dir_getNumFiles(Dir_T dir) {

   assert(dir != NULL);

   return DynArray_getLength(dir->fileC);
}

/* see directory.h for specification */
int Dir_hasDir(Dir_T parent, const char* path, size_t* childID) {

   size_t index;
   int result;
   Dir_T checker;

   assert(parent != NULL);
   assert(path != NULL);

   checker = Dir_create(NULL, path);
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

   assert(parent != NULL);
   assert(path != NULL);

   checkerParent = Dir_create(NULL, path);
   if (checkerParent == NULL)
      return -1;

   checker = File_create(checkerParent, path, NULL, 0);
   if (checker == NULL) {
      free(checkerParent);
      return -1;
   }

   result = DynArray_bsearch(parent->fileC, checker, &index,
                             (int (*)(const void*, const void*))
                             File_compare);

   (void) Dir_destroy(checkerParent);
   (void) File_destroy(checker);

   if (childID != NULL)
      *childID = index;

   return result;
}

/* see directory.h for specification */
Dir_T Dir_getDir(Dir_T dir, size_t childID) {

   assert(dir != NULL);

   if (DynArray_getLength(dir->dirC) > childID)
      return DynArray_get(dir->dirC, childID);

   return NULL;
}

/* see directory.h for specification */
File_T Dir_getFile(Dir_T dir, size_t childID) {

   assert(dir != NULL);

   if (DynArray_getLength(dir->fileC) > childID)
      return DynArray_get(dir->fileC, childID);

   return NULL;
}

/* see directory.h for specification */
Dir_T Dir_getParent(Dir_T dir) {

   assert (dir != NULL);

   return dir->parent;
}

/* see directory.h for specification */
int Dir_linkDir(Dir_T parent, Dir_T child) {

   size_t i;
   char* rest;
   enum {EQUAL};
   
   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerFT_Dir_isValid(parent));
   assert(CheckerFT_Dir_isValid(child));

   if (Dir_hasDir(parent, child->path, NULL) == TRUE)
      return ALREADY_IN_TREE;

   if (Dir_hasFile(parent, child->path, NULL) == TRUE)
      return ALREADY_IN_TREE;

   i = strlen(parent->path);
   if(strncmp(child->path, parent->path, i) != EQUAL)
      return PARENT_CHILD_ERROR;

   rest = child->path + i;
   if (strlen(child->path) >= i && rest[0] != '/')
      return PARENT_CHILD_ERROR;

   rest++;
   if (strstr(rest, "/") != NULL)
      return PARENT_CHILD_ERROR;

   child->parent = parent;

   if (DynArray_bsearch(parent->dirC, child, &i,
                        (int (*)(const void*, const void*))
                        Dir_compare) == 1) {
      return ALREADY_IN_TREE;
   }

   if (DynArray_addAt(parent->dirC, i, child) == TRUE)
      return SUCCESS;

   return PARENT_CHILD_ERROR;
}

/* see directory.h for specification */
int Dir_linkFile(Dir_T parent, File_T child) {

   size_t i;
   const char* rest;
   const char* childPath;
   enum {EQUAL};
   
   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerFT_Dir_isValid(parent));
   

   childPath = File_getPath(child);

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

 if (DynArray_bsearch(parent->fileC, child, &i,
                        (int (*)(const void*, const void*))
                        File_compare) == 1) {
      return ALREADY_IN_TREE;
   }

   if (DynArray_addAt(parent->fileC, i, child) == TRUE) {
      assert(CheckerFT_File_isValid(child));
      return SUCCESS;
   }

   return PARENT_CHILD_ERROR;
}

/* see directory.h for specification */
int Dir_unlinkDir(Dir_T parent, Dir_T child) {

   size_t childID;
   
   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerFT_Dir_isValid(parent));
   assert(CheckerFT_Dir_isValid(child));


   if(DynArray_bsearch(parent->dirC, child, &childID,
                       (int (*)(const void*, const void*))
                       Dir_compare) == 0) {

      assert(CheckerFT_Dir_isValid(parent));
      assert(CheckerFT_Dir_isValid(child));
      return PARENT_CHILD_ERROR;
   }

   (void) DynArray_removeAt(parent->dirC, childID);

   assert(CheckerFT_Dir_isValid(parent));
   assert(CheckerFT_Dir_isValid(child));
   return SUCCESS;

}

/* see directory.h for specification */
int Dir_unlinkFile(Dir_T parent, File_T child) {

   size_t childID;
   
   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerFT_Dir_isValid(parent));
   assert(CheckerFT_File_isValid(child));


   if(DynArray_bsearch(parent->fileC, child, &childID,
                       (int (*)(const void*, const void*))
                       File_compare) == 0) {

      assert(CheckerFT_Dir_isValid(parent));
      assert(CheckerFT_File_isValid(child));
      return PARENT_CHILD_ERROR;
   }

   (void) DynArray_removeAt(parent->fileC, childID);

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

   return strcpy(copyPath, dir->path);
}
