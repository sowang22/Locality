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
