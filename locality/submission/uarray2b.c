/*************************************************************************
 * uarray2b.c
 * Implementation of the blocked uarray2. A blocked uarray2 has smaller
 *      regions within the array (blocks) that are contiguous in memory
 * By Camille Calabrese (ccalab04) and Sophia Wang (swang30)
 * October 2019 for Comp 40, HW3: Locality
 *
 * The UArray2b relies on a one dimensional Hansen UArray_T. The coordinates
 * translation functions maintain the arrangement of elements in the array.
 * Blocks in the array are organized in a column major structure, but
 * cells within a block are organized in a row major structure. For a
 * UArray2b where width % blocksize != 0, there are unused cells which, as
 * an invariant, will remain unused. The coordinates conversion fucntions
 * maintain this invariant by indicating when a coordinate or index is out
 * of bounds.
 *
 * Invariants: Any column coordinate from 0 to width is accessible, and
 *             any row coordinate from 0 to height is accessible. Other
 *             coordinates are inaccessible. Real_width >= width and
 *             real_height >= height. The length of the underlying
 *             UArray_T equals real_width * real_height. The strip of
 *             unused cells around the side of the uarray2b has a width
 *             that's less than blocksize. All int members of the
 *             UArray2b_T struct >= 1.

 *             An especially important invariant is that, within a given
 *             UArray2b, a given one-dimensional index will always be
 *             translated into the same coordinate pair, and a two-dimensional
 *             coordinate pair will always be translated into the same one-
 *             dimensional index.
 *
 *             Another is that blocks of the UArray2b are stored
 *             contiguously in memory.
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

 /*
  * UArray2b_new
  *    Purpose: Creates a new 2D blocked array
  * Parameters: Ints height, width, size, and blocksize
  *    Returns: A pointer to a 2D block array
  *    Expects: Parameters are non-zero.
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

 /*
  * UArray2b_new_64K_block
  *    Purpose: Creates a new 2D blocked array. Calculates the block size
  *             that will be used in the array.
  * Parameters: Ints width, height, and size
  *    Returns: A pointer to a 2D blocked array
  *    Expects: Parameters must be nonzero.
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
        return;
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
 *    Purpose: Mapping function that maps through every element of the 2D
 *             blocked array; visits every cell in 1 block before moving to
 *             another block (block-major)
 * Parameters: A UArray2_T object, apply function, closure argument
 *    Returns: Nothing
 *    Expects: That the parameter is a valid UArray2_T object, that the apply
 *             function works and has valid parameters, and a valid closure
 *             argument that is appropriate to the apply function.
 */
extern void  UArray2b_map(UArray2b_T array2b,
                          void apply(int col, int row, UArray2b_T array2b,
                                     void *elem, void *cl), void *cl)
{
        if (array2b == NULL) {
                RAISE(invalid_input);
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
        return;
}

/*
 * coords_2D_to_1D
 *    Purpose: Converts column and row coordinate into one single index i,
 *             which represents the location in the underlying UArray_T that
 *             corresponds with the specified coordinates
 * Parameters: A UArray2b and ints for the column and row coordinates
 *    Returns: an integer for the one-dimensional index value
 *    Expects: That the UArray2b is valid (checked runtime error), and
 *             expects NOT to be used by client code directly (this is a
 *             private function)
 *       NOTE: Does not necessarily expect that the coordinates are valid
 *             because it can be used to check the validity of coordinates.
 *             returns -1 if coordinates are out of bounds.
 */
int coords_2D_to_1D(UArray2b_T arr, int col, int row)
{
        if (arr == NULL) {
                RAISE(invalid_input);
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

/*
 * coords_1D_to_2D
 *    Purpose: Converts a single integer index, representing a slot in the
 *             underlying UArray_T, to the col and row coordinates that
 *             correspond to that slot
 * Parameters: A UArray2b and an integer index
 *    Returns: A Coordinates struct with the column and row coordinates that
 *             correspond to the specified index
 *    Expects: that the UArray2b is valid, and expects NOT to be called by
 *             the client code directly (this function is private).
 *       NOTE: Does not necessarily expect to receive in-bound coordinates.
 *             This function can be used to check the validity of an index,
 *             and returns {-1, -1} if the index is out of bounds.
 */
struct Coordinates coords_1D_to_2D(UArray2b_T arr, int i)
{
        if (arr == NULL) {
                RAISE(invalid_input);
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
