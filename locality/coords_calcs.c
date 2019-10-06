/***********************************************************************
 *                              coords_calcs.c
 * Assignment: Homework 3 for Comp 40, Fall 2019
 * Authors: Camille Calabrese (ccalab04) and Sophia Wang (swang30)
 * Date: October 6, 2019
 *
 * Functions for calculating new coordinates after rotating, flipping,
 *      or transposing the input coordinates.
 ***********************************************************************/

#include "coords_calcs.h"

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

struct Coordinates flip_hor_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{
        (void) img_height;
        (void) amount;
        c.col = img_width - c.col - 1;
        return c;
}

struct Coordinates flip_ver_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{
        (void) img_width;
        (void) amount;
        c.row = img_height - c.row - 1;
        return c;
}

struct Coordinates rotate_calc(int img_height, int img_width, int amount,
                               struct Coordinates c)
{

        if (!(amount % 90 == 0)) {
                fprintf(stderr, "Rotation must be 0, 90 180 or 270\n");
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

/* NOTE: Does NOT check the validity of the coordinates. Must be done
 *      elsewhere.
 */
struct Coordinates coords_rotate_90(int img_height, struct Coordinates c)
{
        struct Coordinates results = {-1, -1};

        results.col = img_height - c.row - 1;
        results.row = c.col;


        return results;
}

struct Coordinates coords_rotate_180(int img_height, int img_width,
                                     struct Coordinates c)
{
        struct Coordinates results = {-1, -1};
        results.col = img_width  - c.col - 1;
        results.row = img_height - c.row - 1;

        return results;
}
