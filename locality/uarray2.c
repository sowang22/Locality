/***********************************************************************
 *                              uarray2.c
 * Assignment: Homework 2 for Comp 40, Fall 2019
 * Authors: Camille Calabrese (ccalab04) and Andreas Moe (amoe01)
 * Date: September 18, 2019
 *
 * Summary: This file implements the UArray2_T interface described in
 *          UArray2_T.h. It is a 2 dimensional unboxed array.
 ***********************************************************************/

#include "uarray2.h"
#include <uarray.h>
#include <stdlib.h>
#include <except.h>
#include <stdio.h>

struct UArray2_T {
        UArray_T arry;
        int size, width, height;
};
        /* Exceptions */
Except_T Bad_coords = { "Invalid Coordinates" };
Except_T Bad_array  = {"UArray2 object is not working correctly"};
        /* Private functions */
int UArray2_coords_to_index(UArray2_T arr, int col, int row);

/*
 * UArray2_new
 *    Purpose: creates a new UArray2_T object.
 * Parameters: integers for the desired width and height of the array and for
 *             the size of each array element.
 *    Returns: a UArray2_T object.
 *    Expects: That width, height, and elem_size (the Parameters) are all
 *             greater than 0. Furthermore, elem_size should be the size of
 *             some data type.
 */
UArray2_T UArray2_new(int w, int h, int elem_size)
{
        if ((w < 1) || (h < 1)) {
                fprintf(stderr, "%s\n", "Invalid Dimensions.");
                return NULL;
        }
        if (elem_size < 1) {
                fprintf(stderr, "%s\n", "Invalid Element Size.");
                return NULL;
        }
        UArray2_T uarray = malloc(sizeof(struct UArray2_T));

        uarray->arry   = UArray_new((h * w), elem_size);
        uarray->size   = elem_size;
        uarray->width  = w;
        uarray->height = h;

        return uarray;
}

/*
 * UArray2_free
 *    Purpose: Frees all of the heap memory associated with a UArray2_T object
 *             and then sets its address to NULL to prevent further use.
 * Parameters: a pointer to a UArray2_T object
 *    Returns: nothing
 *    Expects: That the parameter is a valid, nonnull UArray2 pointer
 */
void UArray2_free(UArray2_T *arr)
{
        if (arr == NULL || *arr == NULL) {
                return;
        }
        UArray_free(&((*arr)->arry));
        if ((*arr)->arry != NULL) {
                RAISE(Bad_array);
        }

        (*arr)->size   = 0;
        (*arr)->width  = 0;
        (*arr)->height = 0;

        free(*arr);
        *arr = NULL;
}

/*
 * UArray2_size
 *    Purpose: Returns the size of each element in a UArray2.
 * Parameters: A UArray2_T object
 *    Returns: An int for the size of one element in a UArray2_T object.
 *    Expects: that the parameter is a valid UArray2_T object, and that that
 *             UArray2_T object has an element size.
 */
int  UArray2_size(UArray2_T arr)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        return arr->size;
}

/*
 * UArray2_width
 *    Purpose: Returns the width of a UArray2_T object in elements.
 * Parameters: A UArray2_T object.
 *    Returns: An integer for the width of that UArray2_T object, in elements
 *    Expects: That the parameter is a valid UArray2_T object, and that that
 *             UArray2_T object has a width.
 */
int  UArray2_width(UArray2_T arr)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        return arr->width;
}

/*
 * UArray2_height
 *    Purpose: Returns the height of a UArray2_T object in elements.
 * Parameters: A UArray2_T object
 *    Returns: An integer for the height of that UArray2_T object, in elements
 *    Expects: That the parameter is a valid UArray2_T object, and that that
 *             UArray2_T object has a height.
 */
int  UArray2_height(UArray2_T arr)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        return arr->height;

}

/*
 * UArray2_at
 *    Purpose: Returns the address of the element stored at the specified
 *             indices in the specified UArray2.
 * Parameters: A UArray2_T object and the desired coordinates, of type int, in
 *             the order "col", then "row", that is, x and then y where x is
 *             the distance to the right from the top-left element and y is
 *             the distance downward from the top-left element.
 *    Returns: A void pointer of the address of the element at the specified
 *             coordinates.
 *    Expects: That the UArray2_T object is valid. That the integer
 *             coordinates are coordinates within the UArray2_T object, that
 *             is, between -1 and the width/height of the UArray2_T object,
 *             respectively, noninclusive.
 *       NOTE: UArray2_at returns a NULL pointer if the given coordinates are
 *             invalid, and thus can be used to check the validity of
 *             coordinates. Therefore, a UArray2 should not store NULL
 *             pointers if you wish to use this feature.
 */
void *UArray2_at(UArray2_T arr, int col, int row)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        int index;
        void *return_val;
        TRY
                index = UArray2_coords_to_index(arr, col, row);
                return_val = UArray_at(arr->arry, index);
        EXCEPT(Bad_coords)
                return_val = NULL;
        END_TRY;

        return return_val;
}

/*
 * UArray2_map_row_major
 *    Purpose: Runs a specified function on each element in a UArray2_T
 *             object. Traverses the array in the horizontal direction more
 *             quickly than the vertical direction: Left to right, top to
 *             bottom
 * Parameters: A UArray2_T object, a function to apply, and a void pointer to
 *             a closure argument.
 *    Returns: nothing.
 *    Expects: That the UArray2_T object is valid and that the apply function
 *             serves its own intended purpose and has parameters matching the
 *             UArray2_map_row_major function prototype
 */
void UArray2_map_row_major(UArray2_T arr, void apply(int col, int row,
        UArray2_T arr, void *pt1, void *pt2), void *cl)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        int col = 0, row = 0;
        for (row = 0; row < arr->height; row++) {
                for (col = 0; col < arr->width; col++) {
                        apply(col, row, arr, UArray2_at(arr, col, row), cl);
                }
        }
}

/*
 * UArray2_map_col_major
 *    Purpose: Runs a specified function on each element in a UArray2_T
 *             object. Traverses the array in the vertical direction more
 *             quickly than the horizontal direction: Top to bottom, left to
 *             right
 * Parameters: A UArray2_T object, a function to apply, and a void pointer to
 *             a closure argument.
 *    Returns: nothing.
 *    Expects: That the UArray2_T object is valid and that the apply function
 *             serves its own intended purpose and has parameters matching the
 *             UArray2_map_col_major function prototype
 */
void UArray2_map_col_major(UArray2_T arr, void apply(int col, int row,
        UArray2_T arr, void *pt1, void *pt2), void *cl)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        int col = 0, row = 0;
        for (col = 0; col < arr->width; col++) {
                for (row = 0; row < arr->height; row++) {
                        apply(col, row, arr, UArray2_at(arr, col, row), cl);
                }
        }
}

/*
 * UArray2_map_regions
 *    Purpose: Runs the specified function on each element in a UArray2_T
 *             object. Traverses the array one 3x3 region at a time. The
 *             regions within the UArray2 are in column major order, but
 *             each region is internally traversed in row major order.
 * Parameters: The UArray2_T object, the function to be applied, and a
 *             void pointer to a closure argument.
 *    Returns: Nothing
 *    Expects: That the UArray2_T object is valid and that the apply function
 *             serves its own intended purpose and has parameters matching the
 *             UArray2_map_regions function prototype. Also expects that the
 *             closure argument is appropriate for the specified apply
 *             function (unchecked).
 */
void UArray2_map_regions(UArray2_T arr, void apply(int col, int row,
        UArray2_T arr, void *pt1, void *pt2), void *cl)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        int col = 0, row = 0;
        while (col < arr->width && row < arr->height) {
                apply(col, row, arr, UArray2_at(arr, col, row), cl);
                if ((col + 1) % 3 == 0) {
                        if (row == arr->height - 1) {
                                row = 0;
                        }
                        else {
                                row++;
                                col -=3;
                        }
                }
                col++;
        }
}

/*
 * UArray2_coords_to_index
 *    Purpose: Converts col and row coordinates to a single array index
 *             compatible with a Hanson UArray. Col and row can also be
 *             considered the x and y coordinates. Col, or x, measures the
 *             rightward distance from the top-left element. Row, or y,
 *             measures the downward distance from the top-left element.
 * Parameters: A UArray2_T object, an int for the col coordinate, and an int
 *             for the row coordinate
 *    Returns: an int for the corresponding Hanson UArray coordinate
 *    Expects: NOT to be used by client code; rather; to only be called
 *             within other UArray2 functions.
 *             Expects that the UArray2_T object is valid, and that col and
 *             row are valid coordinates, that is, that they are within the
 *             dimensions of the UArray2_T object. In other words, col and row
 *             should be from -1 to the width/height of the UArray2_T,
 *             respectively, noninclusive.
 */
int  UArray2_coords_to_index(UArray2_T arr, int col, int row)
{
        if (arr == NULL) {
                RAISE(Bad_array);
        }
        if (col < 0 || col >= arr->width || row < 0 || row >= arr->height) {
                RAISE(Bad_coords);
        }
        return (row * arr->width) + col;
}
