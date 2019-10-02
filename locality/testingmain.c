#include "uarray2b.h"
#include <stdlib.h>
#include "assert.h"
#include "stdio.h"

struct Coordinates;


int  coords_2D_to_1D(UArray2b_T arr, int col, int row);
void coords_1D_to_2D(UArray2b_T arr, int i);


int main()
{
        UArray2b_T my_arr = UArray2b_new(5, 5, sizeof(int), 2);

        coords_1D_to_2D(my_arr, 2);

        UArray2b_free(&my_arr);
        assert (my_arr == NULL);

        return EXIT_SUCCESS;
}
