/*--------------------------------------------------------------------*/
/* file.c                                                             */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "file.h"
#include "checkerFT.h"

/*
   A file structure represents a file in the file tree
*/
struct file {
   
   /* the full path of this file */
   char* path;

   /* the parent directory of this file */
   Dir_T parent;

   /* the contents of this file */
   void* contents;

   /* the length of the file in bytes */
   size_t length;
};

/* Returns a defensive copy of path or NULL
   if there is an allocation error
*/
static char *File_copyPath(const char *path) {

   char *copy;
   
   assert(path != NULL);

   copy = (char *)malloc(strlen(path) + 1);
   if (copy == NULL)
      return NULL;

   return strcpy(copy, path);
}


/* see file.h for specification */
File_T File_create(Dir_T parent, const char *path, void *contents,
                   size_t length) {

   File_T new_file;
   
   assert(CheckerFT_Dir_isValid(parent));
   assert(path != NULL);
   
   new_file = (File_T)malloc(sizeof(struct file));
   if (new_file == NULL) {
      CheckerFT_Dir_isValid(parent);
      return NULL;
   }

   /* Assigns defensive copy of path to new_file->path */
   new_file->path = File_copyPath(path);
   
   if (new_file->path == NULL)
      return NULL;
   
   new_file->parent = parent;
   new_file->contents = contents;
   new_file->length = length;

   return new_file;
}

/* see file.h for specification */
void File_destroy(File_T file) {

   assert(file != NULL);
   
   free(file->path);
   free(file);
}

/* see file.h for specification*/
int File_compare(File_T file1, File_T file2) {

   assert(file1 != NULL);
   assert(file2 != NULL);

   return strcmp(file1->path, file2->path);
   
}

/* see file.h for specification */
const char* File_getPath(File_T file) {

   assert(file != NULL);

   return file->path;
}

/* see file.h for specification */
Dir_T File_getParent(File_T file) {

   assert(file != NULL);

   return file->parent;

}

/* see file.h for specification */
void *File_getContents(File_T file) {

   assert(file != NULL);

   return file->contents;
}

/* see file.h for specification */
void *File_replaceContents(File_T file, void* newContents,
                           size_t newLength) {

   void* oldContents;

   assert(file != NULL);

   oldContents = file->contents;

   file->contents = newContents;
   file->length = newLength;

   return oldContents;
}

/* see file.h for specification */
size_t File_getLength(File_T file) {

   assert(file != NULL);

   return file->length;
}

/* see file.h for specification */
char* File_toString(File_T file) {
   
   assert(file != NULL);

   return File_copyPath(file->path);
}


















