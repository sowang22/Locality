/***********************************************************************
 *                              coords_calcs.c
 * Assignment: Homework 3 for Comp 40, Fall 2019
 * Authors: Camille Calabrese (ccalab04) and Sophia Wang (swang30)
 * Date: October 6, 2019
 *
 * Functions for calculating new coordinates after rotating, flipping,
 *      or transposing the input coordinates.
 *
 * NOTE: NONE OF THE COORDINATES CALCULATORS IMPLEMENTED BELOW CHECK IF
 * COORDINATES ARE VALID. THIS MUST BE DONE BEFORE CALLING A COORDNATES
 * CALCULATOR. IT IS INTENTIONALLY LEFT UNCHECKED AS THE PRIORITY FOR
 * THESE FUNCTIONS IS SIMPLICITY.
 ***********************************************************************/

#include "coords_calcs.h"

/*
 * transpose_calc
 *    Purpose: Calculates and returns the new coordinates after transposition
 *    Parameters: image height and width, am
 *    Returns: Struct containing the new coordinates
 *    Expects: That the parameter is a valid UArray2_T object, and that that
 *             UArray2_T object has a height.
 */
struct Coordinates transpose_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{
        (void) img_height;
        (void) img_width;
        (void) amount;

        int aux = c.col;
        c.col = c.row;
        c.row = aux;
        return c;
}

/*
 * flip_hor_calc
 *    Purpose: Calculates and returns the new coordinates after flipping the
 *             the image horizontally
 * Parameters: Height and Width of an image, Struct containing Coordinates
 *             to be flipped
 *    Returns: Struct with the new coordinates
 *    Expects: Parameter c (struct coordinates) cannot be NULL and the
 *             width must be a valid number (not 0)
 */
struct Coordinates flip_hor_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{
        (void) img_height;
        (void) amount;
        c.col = img_width - c.col - 1;
        return c;
}

/*
 * flip_ver_calc
 *    Purpose: Calculates and returns the new coordinates after flipping an
 *             image vertically
 * Parameters: Height and Width of an image, Struct containing Coordinates
 *             to be flipped
 *    Returns: Struct with the new coordinates
 *    Expects: Parameter c (struct coordinates) cannot be NULL and the
 *             height must be a valid number (not 0)
 */
struct Coordinates flip_ver_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{
        (void) img_width;
        (void) amount;
        c.row = img_height - c.row - 1;
        return c;
}

/*
 * rotate_calc
 *    Purpose: Handles the rotation of images. This function deals with the
 *             number of degrees to rotate the image and calls the appropriate
 *             functions to actually rotate the image.
 * Parameters: Height and Width of an image, Struct containing Coordinates
 *             to be flipped, int amount which is the degrees to be rotated
 *    Returns: Struct with the new coordinates
 *    Expects: Parameter c (struct coordinates) cannot be NULL and the
 *             degrees to be rotated (amount) must also be in a valid range
 */
struct Coordinates rotate_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{
        if (!(amount % 90 == 0)) {
                fprintf(stderr, "Rotation must be 0, 90, 180, or 270\n");
                EXIT_FAILURE;
        }
        for (int i = 0; i < amount / 90; i++) {
                if (i % 2 == 0) {
                        c = coords_rotate_90(img_height, c);
                }
                else {
                        c = coords_rotate_90(img_width, c);
                }
        }
        return c;
}

/* Does NOT check the validity of coordinates; must be done elsewhere */

/*
 * coords_rotate_90
 *    Purpose: Calculates and returns the new coordinates after a 90 degree
               rotation
 * Parameters: Height of an image, Struct containing Coordinates to be flipped
 *    Returns: Struct with the new coordinates
 *    Expects: Parameter c (struct coordinates) cannot be NULL and the
 *             height must be a valid number (not 0)
 */
struct Coordinates coords_rotate_90(int img_height, struct Coordinates c)
{
        struct Coordinates results = {-1, -1};

        results.col = img_height - c.row - 1;
        results.row = c.col;

        return results;
}

/*
 * coords_rotate_180
 *    Purpose: Calculates and returns the new coordinates after a 180 degree
               rotation
 * Parameters: Height and Width of an image, Struct containing Coordinates
 *             to be flipped
 *    Returns: Struct with the new coordinates
 *    Expects: Parameter c (struct coordinates) cannot be NULL and both the
 *             width and height must be a valid number (not 0)
 *
 *       NOTE: Unused in ppmtrans
 */
struct Coordinates coords_rotate_180(int img_height, int img_width,
                                     struct Coordinates c)
{
        struct Coordinates results = {-1, -1};
        results.col = img_width  - c.col - 1;
        results.row = img_height - c.row - 1;

        return results;
}
