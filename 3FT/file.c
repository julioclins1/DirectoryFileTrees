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
   char* pcPath;

   /* the parent directory of this file */
   Node_T dirParent;

   /* the contents of this file */
   void* pvContent;

   /* the length of the file in bytes */
   size_t uLength;
};

/* Returns a defensive copy of pcPath or NULL
   if there is an allocation error. */
char *File_copyPath(char *pcPath) {

   char *pcCopy;
   
   assert(pcPath != NULL);

   pcCopy = malloc(strlen(pcPath) + 1);

   if (pcCopy == NULL)
      return NULL;

   strcpy(pcCopy, pcPath);

   return pcPath;
}


/* see file.h for specification */
File_T File_create(Dir_T dirParent, char *pcPath, void *pvContent,
                   size_t uLength) {

   assert(CheckerFT_Dir_isValid(parent));
   assert(path != NULL);
   
   new_file = malloc(sizeof(struct file));
   if (new_file == NULL) {
      CheckerFT_Dir_isValid(parent);
      return NULL;
   }

   new_file->pcPath = File_copyPath(pcPath);

   if (new_file->pcPath == NULL)
      return NULL;
   
   new_file->dirParent = dirParent;
   new_file->pvContent = pvContent;
   new_file->uLength = uLength;

   CheckerFT_File_isValid(new_file);
   CheckerFT_Dir_isValid(dirParent);

   return new_file;
}

/* see file.h for specification */
void File_destroy(File_T file) {

   assert(CheckerFT_File_isValid(file));
   
   free(file->pcPath);
   free(file->pvContent);
   free(file);
}

/* see file.h for specification*/
int File_compare(File_T file1, File_T file2) {

   assert(CheckerFT_File_isValid(file1));
   assert(CheckerFT_File_isValid(file2));

   return strcmp(file1->pcPath, file2->pcPath);
   
}

/* see file.h for specification */
const char* File_getPath(File_T file) {

   assert(CheckerFT_File_isValid(file));

   return file->pcPath;
}

/* see file.h for specification */
Dir_T File_getParent(File_T file) {

   assert(CheckerFT_File_isValid(file));

   return file->dirParent;

}

/* see file.h for specification */
void *File_getContent(File_T file) {

   assert(CheckerFT_File_isValid(file));

   return file->pvContent;
}

/* see file.h for specification */
size_t File_getLength(File_T file) {

   assert(CheckerFT_File_isValid(file));

   return file->uLength;
}

/* see file.h for specification */
char* File_toString(File_T file) {

   char *pcCopy;
   
   assert(CheckerFT_File_isValid(file));

   pcCopy = malloc(strlen(file->pcPath + 1));
   if (pcCopy == NULL)
      return NULL;

   return strcpy(pcCopy, file->pcPath);

}


















