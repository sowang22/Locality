/***********************************************************************
 *                          coords_calcs.h
 * Assignment: Homework 3 for Comp 40, Fall 2019
 * Authors: Camille Calabrese (ccalab04) and Sophia Wang (swang30)
 * Date: October 6, 2019
 *
 * Summary: This is a header file containing the function definitions
            for transforming the image. These functions calculate the
            new coordinates and return a struct containing the new coords.
 ***********************************************************************/


#ifndef COORDS_CALCS_H
#define COORDS_CALCS_H

#include "coordinates.h"
#include <stdio.h>
#include <stdlib.h>

struct Coordinates rotate_calc(int img_height, int img_width, int amount,
                               struct Coordinates c);
struct Coordinates coords_rotate_90(int img_height, struct Coordinates c);
struct Coordinates coords_rotate_180(int img_height, int img_width,
                                     struct Coordinates c);

struct Coordinates flip_ver_calc(int img_height, int img_width, int amount,
                               struct Coordinates c);
struct Coordinates flip_hor_calc(int img_height, int img_width, int amount,
                               struct Coordinates c);
struct Coordinates transpose_calc(int img_height, int img_width, int amount,
                               struct Coordinates c);


#endif
