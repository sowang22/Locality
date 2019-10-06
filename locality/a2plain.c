/*************************************************************************
 * a2.c
 * Implementation of the plain uarray2. It is an unboxed, 2 dimensional
 *      array.
 * By Camille Calabrese (ccalab04) and Sophia Wang (swang30)
 * October 2019 for Comp 40, HW3: Locality
 *
 *
 *************************************************************************/

#include <string.h>

#include <a2plain.h>
#include "uarray2.h"
#include "uarray2b.h"

typedef A2Methods_UArray2 A2;

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

static A2Methods_UArray2 new(int width, int height, int size)
{
  return UArray2_new(width, height, size);
}

static A2Methods_UArray2 new_with_blocksize(int width, int height, int size, int blocksize)
{
  (void) blocksize; /* TO-DO: confirm what to actually do */
  return UArray2_new(width, height, size);
}

static void a2free(A2 * a2p)
{
        UArray2_free((UArray2_T *) a2p);
}

static int width(A2 a2)
{
        return UArray2_width(a2);
}

static int height(A2 a2)
{
        return UArray2_height(a2);
}

static int size(A2 a2)
{
        return UArray2_size(a2);
}

static int blocksize(A2 a2)
{
        (void) a2;
        return 1;
}

static A2Methods_Object *at(A2 a2, int i, int j)
{
        return UArray2_at(a2, i, j);
}

/* need typedef here? TODO */

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
  A2Methods_smallapplyfun *apply;
  void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
  struct small_closure *cl = vcl;
  (void)i;
  (void)j;
  (void)uarray2;
  cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
	new_with_blocksize,
	a2free,
	width,
	height,
	size,
	blocksize,
	at,
	map_row_major,
	map_col_major,
	NULL,                   /* map_block_major, */
	map_col_major,   	/* map_default */
	small_map_row_major,
	small_map_col_major,
	NULL,                   /* small_map_block_major */
	small_map_col_major,	// small_map_default
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
