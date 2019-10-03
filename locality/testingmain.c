#include "uarray2b.h"
#include <stdlib.h>
#include "assert.h"
#include "stdio.h"


void fill_array(int col, int row, UArray2b_T array2b, void *elem, void *cl);
void print_for_map_2b(int col, int row, UArray2b_T array2b,
                      void *elem, void *cl);

int main()
{
        UArray2b_T my_arr = UArray2b_new(6, 6, sizeof(int), 2);

        UArray2b_map(my_arr, fill_array, NULL);
        UArray2b_map(my_arr, print_for_map_2b, NULL);

        UArray2b_free(&my_arr);
        assert (my_arr == NULL);

        fprintf(stdout, "\n");

        return EXIT_SUCCESS;
}

void fill_array(int col, int row, UArray2b_T array2b, void *elem, void *cl)
{
 //       fprintf(stdout, "filling array [%d, %d]...\n", col, row);
        int *i = elem;
        *i = row;
        (void) col;
        (void) array2b;
        (void) cl;
}

void print_for_map_2b(int col, int row, UArray2b_T array2b,
                      void *elem, void *cl)
{
        int *i = elem;
        fprintf(stdout, "%d ", *i);
        if ((col + 1) == UArray2b_width(array2b)) {
                fprintf(stdout, "\n");
        }
        (void) row;
        (void) cl;
}
