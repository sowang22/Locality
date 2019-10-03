#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

#include "openfile.h"
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

struct rotate_closure {
        int degrees;
        A2Methods_T methods;
        A2 output;
};

struct Coordinates coords_rotate_90(int img_height, struct Coordinates c);

void rotate(int i, int j, A2Methods_UArray2 array, void *elemm, void *cl);

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
        FILE *image = NULL;
        (void) time_file_name;
        int   rotation       = 0;
        int   i;

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

        A2 out;
        if (rotation % 180 == 0) {
                out = methods->new(pnm->width, pnm->height, sizeof(struct
                                      Pnm_rgb));
        }
        else {
                out = methods->new(pnm->height, pnm->width,
                                      sizeof(struct Pnm_rgb));
        }
        struct rotate_closure cl = {rotation, methods, out};
        methods->map_default(pnm->pixels, rotate, &cl);

        struct Pnm_ppm pnm2 = {methods->width(cl.output),
                               methods->height(cl.output),
                               pnm->denominator, cl.output, methods};

        Pnm_ppmwrite(stdout, &pnm2);

        Pnm_ppmfree(&pnm);
        methods->free(&out);

        return EXIT_SUCCESS;
}


void rotate(int i, int j, A2 array, void *elem, void *cl) {
        struct rotate_closure *closure = cl;
        struct Coordinates new_coords = {i, j};
        struct Pnm_rgb *pixel = elem, *at_p;

        if (!(closure->degrees % 90 == 0)) {
                fprintf(stderr, "Rotation must be 0, 90 180 or 270\n");
                EXIT_FAILURE;
        }
        for (int i = 0; i < closure->degrees / 90; i++) {
                if (i % 2 == 0) {
                        new_coords = coords_rotate_90(
                                closure->methods->height(array), new_coords);
                }
                else {
                        new_coords = coords_rotate_90(
                                closure->methods->width(array), new_coords);
                }
        }

        at_p = closure->methods->at(closure->output, new_coords.col,
                                    new_coords.row);
        *at_p = *pixel;
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

/*  If you have an original image of size w × h, then when the image is rotated 90 degrees, pixel (i, j) in
the original becomes pixel (h − j − 1, i) in the rotated image. */
