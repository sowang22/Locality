/***********************************************************************
 *                              openfile.c
 * Assignment: Homework 2 for Comp 40, Fall 2019
 * Authors: Camille Calabrese (ccalab04) and Andreas Moe (amoe01)
 * Date: September 25, 2019
 *
 * Summary: This is the Implementation of the open_file function. open_file is
 *          commonly used across multiple programs, which is why it is in
 *          its own file.
 *
 ***********
 * EDITED by Camille Calabrese (ccalab04) and Sophia Wang (swang30) on
 * October 3rd for Comp 40 HW3
 *       * Changed Parameters of open_file
 *       * Added load_ppm
 ***********************************************************************/
#include "openfile.h"
#include <stdlib.h>

/* Hanson exception for incorrect input*/
Except_T bad_input = {"Bad Input"};

/*
 * open_file
 *    Purpose: Opens a file from the command line or stdin. Raises an
 *             exception when more than one command line argument is given.
 * Parameters: The number of command line arguments (argc) and the arguments
 *             themselves (argv).
 *    Returns: A file pointer to the open file.
 *    Expects: That only one command line argument is provided, and that the
 *             file exists.
 */
FILE *open_file(char *filename)
{
        FILE *file;

        if (filename != NULL) {
                file = fopen(filename, "r");
        }
        else {
                file = stdin;
        }
        if (file == NULL) {
                fprintf(stderr, "File could not be opened.\n");
                exit(EXIT_FAILURE);
        }
        return file;
}

Pnm_ppm load_ppm(FILE *image_file, A2Methods_T methods)
{
        Pnm_ppm img = Pnm_ppmread(image_file, methods);
        if (methods->width(img->pixels) < 1 ||
            methods->height(img->pixels) < 1) {
                RAISE(bad_input);
        }
        if (image_file != stdin) {
                fclose(image_file);
        }
        return img;
}
