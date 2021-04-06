
/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "checkerDT.h"

/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T n) {
   Node_T parent;
   const char* npath;
   const char* ppath;
   const char* rest;
   size_t i;

   /* A NULL pointer is not a valid node */
   if(n == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }


   /* Check that node's path is not NULL */
   npath = Node_getPath(n);
   if (npath == NULL) {
      fprintf(stderr, "A node's path is NULL\n");
      return FALSE;
   }

   parent = Node_getParent(n);
   if(parent != NULL) {
      
      /* Check that parent's path must be prefix of n's path */
      ppath = Node_getPath(parent);
      i = strlen(ppath);
      if(strncmp(npath, ppath, i)) {
         fprintf(stderr, "P's path is not a prefix of C's path\n");
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
static boolean CheckerDT_treeCheck(Node_T n) {
   size_t c;
   Node_T nParent;
   Node_T child;
   Node_T nPrev;

   if(n != NULL) {
    
      /* Check on each non-root node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerDT_Node_isValid(n))
         return FALSE;

      /* Placeholder. nPrev will be used to check the invariance
         that the subdirectories should be in sorted order */
      nPrev = n;

      for(c = 0; c < Node_getNumChildren(n); c++)
      {
         child = Node_getChild(n, c);
         nParent = Node_getParent(child);

         /* If node's child has a different parent, return FALSE */
         if (nParent != n) {
            fprintf(stderr, "Node points to wrong parent\n");
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerDT_treeCheck(child))
            return FALSE;
         
         /* if nodes are not in sorted order, return FALSE */
         if (Node_compare(nPrev, child) >= 0) {
            fprintf(stderr, "Subdirectories are not in sorted order\n");
            return FALSE;
         }
         nPrev = child;
      }
   }
      
   return TRUE;
}



/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean isInit, Node_T root, size_t count) {
   
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
         
         if (Node_getParent(root) != NULL) {
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
   return CheckerDT_treeCheck(root);

}
