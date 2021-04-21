/*--------------------------------------------------------------------*/
/* checkerFT.c                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

/* CheckerFT is a module that is called at the beginning and end of
   each function in ft.c (and its relatives), checking the file
   tree invariants */

#include "defs.h"
#include "dynarray.h"
#include "checkerFT.h"
#include "a4def.h"

/* see checkerFT.h for specification */
boolean CheckerFT_File_isValid(File_T file) {

   Dir_T parent;
   const char* path;
   const char* parentPath;

   /* a NULL pointer is not a valid file */
   if (file == NULL) {
      fprintf(stderr, "A file is NULL\n");
      return FALSE;
   }

   path = File_getPath(file);

   /* Files cannot have NULL paths */
   if (path == NULL) {
      fprintf(stderr, "A file's path is NULL\n");
      return FALSE;
   }

   parent = File_getParent(file);

   /* Files cannot have null parents */
   if (parent == NULL) {
      fprintf(stderr, "A file's parent is NULL\n");
      return FALSE;
   }

   /* Parent should point back to child */
   if (Dir_hasChild(parent, path, NULL, FILES) == FALSE) {
      fprintf(stderr, "Parent does not point back to child file\n");
      return FALSE;
   }

   /* Parent cannot have any children, file or dir, with same path */
   if (Dir_hasChild(parent, path, NULL, DIR) == TRUE) {
      fprintf(stderr, "P has a child dir with same path as file\n");
      return FALSE;
   }
      
   parentPath = Dir_getPath(parent);

   /* Parent's path should be a prefix of child's path */
   if (strncmp(parentPath, path, strlen(parentPath)) != EQUAL) {
      fprintf(stderr, "P's path is not a prefix of C's path\n");
      fprintf(stderr, "Parent: %s, Child: %s", parentPath, path);
      return FALSE;
   }
   
   return TRUE;
}

/* see checkerFT.h for specification */
boolean CheckerFT_Dir_isValid(Dir_T dir) {
   
   Dir_T parent;
   const char* path;
   const char* parentPath;
   const char* rest;
   size_t i;

   /* A NULL pointer is not a valid directory */
   if(dir == NULL) {
      fprintf(stderr, "A directory is a NULL pointer\n");
      return FALSE;
   }


   /* Check that directory's path is not NULL */
   path = Dir_getPath(dir);
   if (path == NULL) {
      fprintf(stderr, "A directory's path is NULL\n");
      return FALSE;
   }

   parent = Dir_getParent(dir);
   if(parent != NULL) {
      
      /* Check that parent's path must be prefix of dir's path */
      parentPath = Dir_getPath(parent);
      i = strlen(parentPath);
      if(strncmp(path, parentPath, i) != EQUAL) {
         fprintf(stderr, "P's path is not a prefix of C's path\n");
         fprintf(stderr, "Parent: %s, Child: %s\n", parentPath, path);
         
         return FALSE;
      }
      /* Check that dir's path after parent's path + '/'
         must have no further '/' characters */
      rest = path + i;
      rest++;
      if(strstr(rest, "/") != NULL) {
         fprintf(stderr, "C's path has grandchild of P's path\n");
         return FALSE;
      }
   }

   return TRUE;
}

/* Performs a pre-order traversal of the tree rooted at dir.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise. */
static boolean CheckerFT_treeCheck(Dir_T dir) {
   size_t c;
   Dir_T parent;
   Dir_T childDir;
   File_T childFile;
   const char* parentPath;
   const char* prevPath;
   const char* currPath;

   if (dir != NULL) {
    
      /* Check on each non-root directory: directory must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerFT_Dir_isValid(dir))
         return FALSE;

      parentPath = Dir_getPath(dir);

      /* Placeholder. prevPath will be used to check the invariance
         that files and directories should be in sorted order by path
         name */
      prevPath = parentPath;

      for(c = 0; c < Dir_getNumChildren(dir, FILES); c++) {

         /* Check each child file of dir: file must be valid */
         childFile = Dir_getChild(dir, c, FILES);

         if(CheckerFT_File_isValid(childFile) == FALSE)
            return FALSE;

         currPath = File_toString(childFile);

         /* File should point to the correct parent */
         if (File_getParent(childFile) != dir) {
            fprintf(stderr, "A file points to the wrong parent\n");
            fprintf(stderr, "Parent: %s, Child: %s\n",
                    parentPath,currPath);
         }

         /* Files should be sorted in lexicographic order by path */
         if (strcmp(prevPath, currPath) >= 0) {
            fprintf(stderr, "Files are not in sorted order\n");
            fprintf(stderr, "Parent: %s\n", parentPath);
         }

         prevPath = currPath;
      }

      prevPath = parentPath;
      for(c = 0; c < Dir_getNumChildren(dir, DIR); c++)
      {
         childDir = Dir_getChild(dir, c, DIR);
         parent = Dir_getParent(childDir);

         /* If dir's child has a different parent, return FALSE */
         if (parent != dir) {
            fprintf(stderr, "Directory  points to wrong parent\n");
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(CheckerFT_treeCheck(childDir) != TRUE)
            return FALSE;

         currPath = Dir_getPath(childDir);
         
         /* if nodes are not in sorted order, return FALSE */
         if (strcmp(prevPath, currPath) >= 0) {
            fprintf(stderr, "Subdirectories are not in sorted order\n");
            fprintf(stderr, "Parent: %s\n", parentPath);
            return FALSE;
         }
         prevPath = currPath;
      }
   }
      
   return TRUE;
}



/* see checkerFT.h for specification */
boolean CheckerFT_isValid(boolean isInit, Dir_T root, size_t count) {
   
   /* Checks invariants for tree */
   if (!isInit) {
      
      if (count != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }
   
      if (root != NULL) {
         fprintf(stderr, "Not initialized, but root is not NULL\n");
         return FALSE;
      }
   }

   else {

      if (root != NULL) {

         if (count == 0) {
            fprintf(stderr, "Root is not NULL but count is 0\n");
            return FALSE;
         }
         
         if (Dir_getParent(root) != NULL) {
            fprintf(stderr, "Root's parent is not null\n");
            return FALSE;
         }
      }

      else {
         if (count > 0) {
            fprintf(stderr, "Count is more than 0, but root is NULL\n");
            return FALSE;
         } 
      }
   }

   /* Now checks invariants recursively at each node from the root. */
   return CheckerFT_treeCheck(root);

}
