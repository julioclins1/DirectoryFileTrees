/*--------------------------------------------------------------------*/
/* checkerFT.c                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

/* CheckerFT is a module that is called at the beginning and end of
   each function in dt.c (and its relatives), checking the directory
   tree invariants */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "dynarray.h"
#include "checkerFT.h"


/* see checkerFT.h for specification */
boolean CheckerFT_File_isValid(File_T file) {

   Dir_T parent;
   const char* path;
   const char* parentPath;
   enum {EQUAL};

   if (file == NULL)
      return FALSE;

   path = File_getPath(file);

   if (path == NULL)
      return FALSE;

   parent = File_getParent(file);

   if (parent == NULL)
      return FALSE;

   if (Dir_hasFile(parent, path, NULL) == FALSE)
      return FALSE;

   parentPath = Dir_getPath(parent);

   if (strncmp(parentPath, path, strlen(parentPath)) != EQUAL)
      return FALSE;
   
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerFT_Dir_isValid(Dir_T dir) {
   Dir_T parent;
   const char* npath;
   const char* ppath;
   const char* rest;
   size_t i;

   /* A NULL pointer is not a valid node */
   if(dir == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }


   /* Check that node's path is not NULL */
   npath = Dir_getPath(dir);
   if (npath == NULL) {
      fprintf(stderr, "A node's path is NULL\n");
      return FALSE;
   }

   parent = Dir_getParent(dir);
   if(parent != NULL) {
      
      /* Check that parent's path must be prefix of n's path */
      ppath = Dir_getPath(parent);
      i = strlen(ppath);
      if(strncmp(npath, ppath, i)) {
         fprintf(stderr, "P's path is not a prefix of C's path\n");
         fprintf(stderr, "Parent: %s, Child: %s\n", ppath, npath);
         
         return FALSE;
      }
      /* Check that n's path after parent's path + '/'
         must have no further '/' characters */
      rest = npath + i;
      rest++;
      if(strstr(rest, "/") != NULL) {
         fprintf(stderr, "C's path has grandchild of P's path\n");
         return FALSE;
      }
   }

   return TRUE;
}

/* Performs a pre-order traversal of the tree rooted at n.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise. */
static boolean CheckerFT_treeCheck(Dir_T n) {
   size_t c;
   Dir_T nParent;
   Dir_T child;
   Dir_T nPrev;

   if(n != NULL) {
    
      /* Check on each non-root node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerFT_Dir_isValid(n))
         return FALSE;

      /* Placeholder. nPrev will be used to check the invariance
         that the subdirectories should be in sorted order */
      nPrev = n;

      for(c = 0; c < Dir_getNumDir(n); c++)
      {
         child = Dir_getDir(n, c);
         nParent = Dir_getParent(child);

         /* If node's child has a different parent, return FALSE */
         if (nParent != n) {
            fprintf(stderr, "Node points to wrong parent\n");
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerFT_treeCheck(child))
            return FALSE;
         
         /* if nodes are not in sorted order, return FALSE */
         if (Dir_compare(nPrev, child) >= 0) {
            fprintf(stderr, "Subdirectories are not in sorted order\n");
            return FALSE;
         }
         nPrev = child;
      }
   }
      
   return TRUE;
}



/* see checkerDT.h for specification */
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
