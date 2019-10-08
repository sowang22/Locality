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
#include "assert.h"
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

 /*
  * UArray2b_new
  *    Purpose: Creates a new 2D blocked array
  * Parameters: Ints height, width, size, and blocksize
  *    Returns: A pointer to a 2D block array
  *    Expects: Parameters are non-zero.
  */
extern UArray2b_T UArray2b_new(int w, int h, int size, int blocksize)
{
        assert(w != 0 && h != 0 && size != 0 && blocksize != 0);

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
        assert(w != 0 && h != 0 && size != 0);
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
 *    Purpose: Frees the memory associated with the array
 * Parameters: A UArray2b_T object
 *    Returns: Nothing
 *    Expects: That the parameter is a valid UArray2_T object, and that that
 *             UArray2_T object has a height.
 */
extern void UArray2b_free(UArray2b_T *array2b)
{
        assert(array2b != NULL); // might have to do the address and parameter comment
        UArray_free(&((*array2b)->array));
        free(*(array2b));
        *array2b = NULL;
        return;
}

/*
 * UArray2b_width
 *    Purpose: Returns the width of a UArray2b_T object
 * Parameters: A UArray2b_T object
 *    Returns: The width of the UArray2b_T object
 *    Expects: UArray2b_T object cannot be NULL
 */
extern int UArray2b_width(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/*
 * UArray2b_height
 *    Purpose: Returns the height of a UArray2_T object
 * Parameters: A UArray2b_T object
 *    Returns: The height of the UArray2b_T object
 *    Expects: UArray2b_T object cannot be NULL
 */
extern int UArray2b_height(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/*
 * UArray2b_size
 *    Purpose: Returns the size of an element of an UArray2b_T object
 * Parameters: A UArray2b_T object
 *    Returns: An integer for the size of an element in the UArray2b_T object
 *    Expects: UArray2b_T object cannot be NULL
 */
extern int UArray2b_size(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->elem_size;
}

/*
 * UArray2b_blocksize
 *    Purpose: Returns the blocksize of a UArray2b_T object
 * Parameters: A UArray2_T object
 *    Returns: Size of the block in the UArray2b_T object
 *    Expects: UArray2b_T object cannot be NULL
 */
extern int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

 /*
  * UArray2b_at
  *    Purpose: Returns the pointer to the cell in the given col and row
  * Parameters: A UArray2b_T object, int column and row
  *    Returns: Void pointer to the value stored at a certain index
  *    Expects: That the parameter is a valid UArray2_T object and the row and
  *             column values passed in must be valid.
  *       Note: can be used to check if coordinates are valid. Returns a NULL
  *             pointer if coordinates are not valid. Therefore, expects that
  *             the array does not store any null pointers.
  */
extern void *UArray2b_at(UArray2b_T array2b, int col, int row)
{
        assert(array2b != NULL);
        if (coords_2D_to_1D(array2b, col, row) == -1) {
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
 *    Expects: That the parameter is a valid UArray2_T object, apply function
 *             with its valid parameters, and a valid closure argument.
 */
extern void  UArray2b_map(UArray2b_T array2b,
                          void apply(int col, int row, UArray2b_T array2b,
                                     void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
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
  *    Purpose: Converts the 2D array coordinates to the coordinates in the
  *             1D representation.
  * Parameters: A UArray2_T object, int col and row
  *    Returns: Index/Coordinate of the element in the 1D array
  *    Expects: UArray2b_T object not NULL, col and row must be valid coords
  *       Note: Can be used to check if coordinates are valid. Returns -1 for
  *             invalid coordinates
  */
int coords_2D_to_1D(UArray2b_T arr, int col, int row)
{
        assert(arr != NULL);
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
  *    Purpose: Converts the 1D array index to the 2D array coordinates
  * Parameters: A UArray2_T object, int i (index)
  *    Returns: Struct Coordinates containing the 2D coordinates
  *    Expects: UArray2b_T must be non-NULL, 1D index must be valid
  *       Note: Can be used to check if a coordinate is valid.
                Returns {-1, -1} if coordinate is invalid.
  */
struct Coordinates coords_1D_to_2D(UArray2b_T arr, int i)
{
        assert(arr != NULL);
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
