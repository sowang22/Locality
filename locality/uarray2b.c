/*************************************************************************
 * uarray2b.c
 * Implementation of the blocked uarray2. A blocked uarray2 has smaller
 *      regions within the array (blocks) that are contiguous in memory
 * By Camille Calabrese (ccalab04) and Sophia Wang (swang30)
 * October 2019 for Comp 40, HW3: Locality
 *
 *
 *************************************************************************/

#include "uarray2b.h"
#include "uarray.h"
#include "coordinates.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const int KILOBYTE = 1024;

struct UArray2b_T {
        int width, height, blocksize, elem_size, real_width, real_height;
        UArray_T array;
};

        /* Private function prototypes */
int coords_2D_to_1D(UArray2b_T arr, int col, int row);
struct Coordinates coords_1D_to_2D(UArray2b_T arr, int i);


/*
 * new blocked 2d array
 * blocksize = square root of # of cells in block.
 * blocksize < 1 is a checked runtime error
 */
extern UArray2b_T UArray2b_new(int w, int h, int size, int blocksize)
{
        UArray2b_T aux = malloc(sizeof(struct UArray2b_T));

        aux->width     = w;
        aux->height    = h;
        aux->elem_size = size;
        aux->blocksize = blocksize;

        aux->real_width  = w + (aux->blocksize - (w % aux->blocksize));
        aux->real_height = h + (aux->blocksize - (h % aux->blocksize));
        if (w % aux->blocksize == 0) {
                aux->real_width = w;
        }
        if (h % aux->blocksize == 0) {
                aux->real_height = h;
        }
        aux->array     = UArray_new(aux->real_width * aux->real_height, size);

        return aux;
}

/* new blocked 2d array: blocksize as large as possible provided
 * block occupies at most 64KB (if possible)
 */
extern UArray2b_T UArray2b_new_64K_block(int w, int h, int size)
{
        UArray2b_T aux = malloc(sizeof(struct UArray2b_T));
        int blocksize_sq = 64 * KILOBYTE / size;

        aux->width     = w;
        aux->height    = h;
        aux->elem_size = size;
        aux->blocksize = sqrt(blocksize_sq);

        if (aux->blocksize < 1) { /* in case one elem is over 64KB */
                aux->blocksize = 1;
        }
        aux->real_width  = w + (aux->blocksize - (w % aux->blocksize));
        aux->real_height = h + (aux->blocksize - (h % aux->blocksize));

        aux->array     = UArray_new(aux->real_width * aux->real_height, size);

        return aux;
}

extern void UArray2b_free(UArray2b_T *array2b)
{
        UArray_free(&((*array2b)->array));
        free(*(array2b));
        *array2b = NULL;
}

extern int UArray2b_width(UArray2b_T array2b)
{
        return array2b->width;
}

extern int UArray2b_height(UArray2b_T array2b)
{
        return array2b->height;
}

extern int UArray2b_size(UArray2b_T array2b)
{
        return array2b->elem_size;
}

extern int UArray2b_blocksize(UArray2b_T array2b)
{
        return array2b->blocksize;
}

/* return a pointer to the cell in the given column and row.
 * index out of range is a checked run-time error
 *
 * Note: can be used to check if coordinates are valid. Returns a NULL
 *      pointer if coordinates are not valid. Therefore, expects that the
 *      array does not store any null pointers.
 */
extern void *UArray2b_at(UArray2b_T array2b, int col, int row)
{
        if (coords_2D_to_1D(array2b, col, row) == -1) {
                return NULL;
        }
        else {
                return UArray_at(array2b->array, coords_2D_to_1D(array2b,
                                                                col, row));
        }
}

/* visits every cell in one block before moving to another block */
extern void  UArray2b_map(UArray2b_T array2b,
                          void apply(int col, int row, UArray2b_T array2b,
                                     void *elem, void *cl), void *cl)
{
        struct Coordinates coords;
        for (int i = 0; i < array2b->real_width * array2b->real_height; i++) {
                coords = coords_1D_to_2D(array2b, i);
                if (coords.col != -1 && coords.row != -1) {
                        apply(coords.col, coords.row, array2b,
                              UArray2b_at(array2b, coords.col, coords.row),
                              cl);
                }
        }
}


/* Note: can be used to check if coordinates are valid. Returns -1 for
 * invalid coordinates */
int coords_2D_to_1D(UArray2b_T arr, int col, int row)
{
        if (col < 0 || col >= arr->width ||
            row < 0 || row >= arr->height) {
                    return -1;
            }

        int block_col = col / arr->blocksize,
            block_row = row / arr->blocksize,
            block_height = arr->real_height / arr->blocksize,
            index = 0;

        index += (((block_col * block_height) + block_row)
                  * arr->blocksize * arr->blocksize);
        row %= arr->blocksize;
        col %= arr->blocksize;
        index += (row * arr->blocksize) + col;


        return index;
}

/* Note: Can be used to check if a coordinate is valid. Returns {-1, -1} if
 * coordinate is invalid. */
struct Coordinates coords_1D_to_2D(UArray2b_T arr, int i)
{
        struct Coordinates c = {-1, -1};
        if (i < 0 || i >= arr->real_width * arr->real_height) {
                return c;
        }
        int block = 0, row = 0, col = 0;

        block = i / (arr->blocksize * arr->blocksize);
        i %= (arr->blocksize * arr->blocksize);

        col += block / (arr->real_height / arr->blocksize);
        row += block % (arr->real_height / arr->blocksize);

        col *= arr->blocksize;
        row *= arr->blocksize;

        row += i / arr->blocksize;
        col += i % arr->blocksize;

        if (!(col < 0 || row < 0 || col >= arr->width || row >= arr->height)) {
                c.col = col;
                c.row = row;
        }

        return c;
}
