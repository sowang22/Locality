/***********************************************************************
 *                              openfile.h
 * Assignment: Homework 2 for Comp 40, Fall 2019
 * Authors: Camille Calabrese (ccalab04) and Andreas Moe (amoe01)
 * Date: September 25, 2019
 *
 * Summary: This is the interface of the open_file function. open_file is
 *          commonly used across multiple programs, which is why it is in
 *          its own file.
 *
 ***********
 * EDITED by Camille Calabrese (ccalab04) and Sophia Wang (swang30) on
 * October 3rd for Comp 40 HW3
 *       * changed Parameters of open_file
 *       * added load_ppm
 ***********************************************************************/

#include <stdio.h>
#include <except.h>

#include "a2methods.h"
#include "pnm.h"

FILE  *open_file(char *filename);
Pnm_ppm load_ppm(FILE *image_file, A2Methods_T methods);
