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
#include "except.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const int KILOBYTE = 1024;

struct UArray2b_T {
        int width, height, blocksize, elem_size, real_width, real_height;
        UArray_T array;
};

Except_T invalid_input = {"Invalid Parameter"};

        /* Private function prototypes */
int coords_2D_to_1D(UArray2b_T arr, int col, int row);
struct Coordinates coords_1D_to_2D(UArray2b_T arr, int i);


/*
 * UArray2b_new
 *    Purpose: Creates a new blocked 2D array
 * Parameters: width, height, the size of one element in bytes, and the
 *             blocksize (square root of the number of elements in a block)
 *    Returns: The blocked 2D array
 *    Expects: That width, height, size, and blocksize are all at least 1
 *             (checked runtime error)
 */
extern UArray2b_T UArray2b_new(int w, int h, int size, int blocksize)
{
        if (w < 1 || h < 1 || size < 1 || blocksize < 1) {
                RAISE(invalid_input);
        }
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

/*
 * UArray2b_new_64K_block
 *    Purpose: Creates a new blocked 2D array, where each block of the array
 *             is as large as possible while totaling 64KB or less. If an
 *             element is more than 64KB, blocksize is 1.
 * Parameters: width, height, and element size of the array
 *    Returns: The blocked 2D array
 *    Expects: That width, height, and size are all at least one
 */
extern UArray2b_T UArray2b_new_64K_block(int w, int h, int size)
{
        if (w < 1 || h < 1 || size < 1) {
                RAISE(invalid_input);
        }
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

/*
 * UArray2b_free
 *    Purpose: Frees the memory associated with a blocked 2D array
 * Parameters: a pointer to a UArray2b_T
 *    Returns: nothing
 *    Expects: That the pointer passed in as a parameter is valid and points
 *             to a UArray2b_T
 */
extern void UArray2b_free(UArray2b_T *array2b)
{
        if (array2b == NULL || *array2b == NULL) {
                return;
        }
        UArray_free(&((*array2b)->array));
        free(*(array2b));
        *array2b = NULL;
}

/*
 * UArray2b_width
 *    Purpose: Returns the width of a blocked 2D array
 * Parameters: a UArray2b_T
 *    Returns: the width of that UArray2b_T
 *    Expects: That the pointer passed in is valid
 */
extern int UArray2b_width(UArray2b_T array2b)
{
        if (array2b == NULL) {
                RAISE(invalid_input);
        }
        return array2b->width;
}

/*
 * UArray2b_height
 *    Purpose: Returns the height of a blocked 2D array
 * Parameters: a UArray2b_T
 *    Returns: the height of that UArray2b_T
 *    Expects: That the pointer passed in is valid
 */
extern int UArray2b_height(UArray2b_T array2b)
{
        if (array2b == NULL) {
                RAISE(invalid_input);
        }
        return array2b->height;
}

/*
 * UArray2b_size
 *    Purpose: Returns the element size of a blocked 2D array
 * Parameters: a UArray2b_T
 *    Returns: the element size of that UArray2b_T
 *    Expects: That the pointer passed in is valid
 */
extern int UArray2b_size(UArray2b_T array2b)
{
        if (array2b == NULL) {
                RAISE(invalid_input);
        }
        return array2b->elem_size;
}

/*
 * UArray2b_blocksize
 *    Purpose: Returns the block size of a blocked 2D array
 * Parameters: a UArray2b_T
 *    Returns: the block size of that UArray2b_T
 *    Expects: That the pointer passed in is valid
 */
extern int UArray2b_blocksize(UArray2b_T array2b)
{
        if (array2b == NULL) {
                RAISE(invalid_input);
        }
        return array2b->blocksize;
}

/*
 * UArray2b_at
 *    Purpose: Gets the element at the specified indices of the specified
 *             array
 * Parameters: A blocked 2D array and column and row coordinates for the
 *             desired element
 *    Returns: A void pointer to the specified element
 *    Expects: That the coordinates are in bounds of the array and the array
 *             is valid (especially that it is nonnull). Checked runtime
 *             errors
 */
extern void *UArray2b_at(UArray2b_T array2b, int col, int row)
{
        if (array2b == NULL) {
                RAISE(invalid_input);
                return NULL;
        }
        if (coords_2D_to_1D(array2b, col, row) == -1) {
                RAISE(invalid_input);
                return NULL;
        }
        else {
                return UArray_at(array2b->array, coords_2D_to_1D(array2b,
                                                                col, row));
        }
}

/*
 * UArray2b_map
 *    Purpose: Calls the specified function on each cell in a UArray2b. Visits
 *             every cell in one block before moving to the next block.
 * Parameters: A UArray2b, a function to apply to each cell, and a void
 *             pointer to a closure argument
 *    Returns: Nothing
 *    Expects: That the UArray2b is valid, that the apply function works and
 *             can be applied to the elements of the UArray2b, and that the
 *             closure argument is appropriate to the apply function
 */
extern void  UArray2b_map(UArray2b_T array2b,
                          void apply(int col, int row, UArray2b_T array2b,
                                     void *elem, void *cl), void *cl)
{
        if (array2b == NULL) {
                RAISE(bad_input);
        }
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

/*
 * coords_2D_to_1D
 *    Purpose: Converts column and row coordinate into one single index i,
 *             which represents the location in the underlying UArray_T that
 *             corresponds with the specified coordinates
 * Parameters: A UArray2b and ints for the column and row coordinates
 *    Returns: an integer for the one-dimensional index value
 *    Expects: That the UArray2b is valid (checked runtime error)
 *       NOTE: Does not necessarily expect that the coordinates are valid
 *             because it can be used to check the validity of coordinates.
 *             returns -1 if coordinates are out of bounds.
 */
/* Note: can be used to check if coordinates are valid. Returns -1 for
 * invalid coordinates */
int coords_2D_to_1D(UArray2b_T arr, int col, int row)
{
        if (arr == NULL) {
                RAISE(bad_input);
        }
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
        if (arr == NULL) {
                RAISE(bad_input);
        }
        struct Coordinates c = {-1, -1};
        if (i < 0 || i >= (arr->real_width * arr->real_height)) {
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
