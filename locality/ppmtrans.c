#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

#include "openfile.h"
#include "coords_calcs.h"
#include "coordinates.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

typedef A2Methods_UArray2 A2;

static const int TRANSPOSE_CODE = 1;
static const int FLIP_HOR_CODE = 2;
static const int FLIP_VER_CODE = 3;

struct transform_closure {
        int amount;
        A2Methods_T methods;
        A2 output;
        struct Coordinates (*coords_calc)(int img_height, int img_width,
                                          int amount, struct Coordinates c);
};

void transform(int i, int j, A2 array, void *elemm, void *cl);
A2 make_a2_out(int rotation, A2Methods_T methods, Pnm_ppm pic);
void assign_coords_calc(struct transform_closure *cl);

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

int main(int argc, char *argv[])
{
        char *time_file_name = NULL, *img_file_name = NULL;
        FILE *image = NULL, *timer_out = NULL;
        int   rotation       = -1;
        int   i;
        CPUTime_T timer = NULL;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
				    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
				    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr,
					"Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        rotation = TRANSPOSE_CODE;
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no flip spec */
                                usage(argv[0]);
                        }
                        char *flip_spec = argv[++i];
                        if (strcmp(flip_spec, "horizontal") == 0) {
                                rotation = FLIP_HOR_CODE;
                        }
                        else if (strcmp(flip_spec, "vertical") == 0) {
                                rotation = FLIP_VER_CODE;
                        }
                        else {
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
				argv[i]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        if (argc - i == 1) { /* if file name is on command line, get it */
                img_file_name = argv[argc - 1];
        }
        image = open_file(img_file_name);
        Pnm_ppm pnm = load_ppm(image, methods);

        A2 out = make_a2_out(rotation, methods, pnm);

        struct transform_closure cl = {rotation, methods, out, NULL};
        assign_coords_calc(&cl);

        if (time_file_name != NULL) {
                timer = CPUTime_New();
                timer_out = fopen(time_file_name, "w");
                CPUTime_Start(timer);
        }
        map(pnm->pixels, transform, &cl);

        if (timer != NULL) {
                double total_time = CPUTime_Stop(timer),
                       time_per_p = total_time / (pnm->width * pnm->height);
                fprintf(timer_out, "%0f\n%0f\n", total_time, time_per_p);
                fclose(timer_out);
                CPUTime_Free(&timer);
        }

        struct Pnm_ppm pnmout = {methods->width(cl.output),
                                 methods->height(cl.output),
                                 pnm->denominator, cl.output, methods};
        Pnm_ppmwrite(stdout, &pnmout);

        Pnm_ppmfree(&pnm);
        methods->free(&out);

        return EXIT_SUCCESS;
}

void transform(int i, int j, A2 array, void *elem, void *cl) {
        struct transform_closure *closure = cl;
        struct Coordinates new_coords = {i, j};
        struct Pnm_rgb *pixel = elem, *at_p;

        new_coords = closure->coords_calc(closure->methods->height(array),
                             closure->methods->width(array), closure->amount,
                             new_coords);


        at_p = closure->methods->at(closure->output, new_coords.col,
                                    new_coords.row);
        *at_p = *pixel;
}

A2 make_a2_out(int rotation, A2Methods_T methods, Pnm_ppm pic)
{
        A2 result;
        if (rotation % 180 == 0 || rotation == 0 ||
            rotation == FLIP_HOR_CODE || rotation == FLIP_VER_CODE ) {
                result = methods->new(pic->width, pic->height, sizeof(struct
                                      Pnm_rgb));
        }
        else {
                result = methods->new(pic->height, pic->width,
                                      sizeof(struct Pnm_rgb));
        }
        return result;
}

void assign_coords_calc(struct transform_closure *cl)
{
        if (cl->amount == 0 || cl->amount % 90 == 0) {
                cl->coords_calc = rotate_calc;
        } else if (cl->amount == TRANSPOSE_CODE) {
                cl->coords_calc = transpose_calc;
        } else if (cl->amount == FLIP_HOR_CODE) {
                cl->coords_calc = flip_hor_calc;
        } else if (cl->amount == FLIP_VER_CODE) {
                cl->coords_calc = flip_ver_calc;
        }
}
