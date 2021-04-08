/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Author: Julio Lins and Rishabh Rout                                */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "dynarray.h"
#include "ft.h"
#include "directory.h"
#include "file.h"
#include "checkerFT.h"

/* A File Tree is an AO with 3 state variables: */
/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitialized;
/* a pointer to the root directory in the hierarchy */
static Node_T root;
/* a counter of the number of directories and files in the hierarchy */
static size_t count;
