/* PMSIS includes */
#include "pmsis.h"




struct cl_args_s
{
    uint32_t size;
    /* L1 Buffers */
    uint8_t *l1_buffer;
    uint8_t *l1_buffer2;

    /*L2 Buffers*/
    uint8_t *l2_in;
    uint8_t *l2_in2;


    uint8_t *l2_out;
    uint8_t *l2_out2;
};


/**************************************************************
 ******************** FUNCTIONS DECLARATIONS ******************
 **************************************************************/
extern uint8_t** list_to_matrice(uint32_t raw, uint32_t colums, uint8_t* buffer);

extern uint8_t* matrice_to_list(uint32_t raw, uint32_t colums, uint8_t** buffer);

extern uint8_t** addMatrices(uint8_t** matrix1, uint8_t** matrix2, uint32_t raw, uint32_t colums);

extern uint8_t** multiplySquareMatrices(uint8_t** matrix1, uint8_t** matrix2, uint32_t size);

extern void printMatrix(int** matrix, int rows, int columns);

