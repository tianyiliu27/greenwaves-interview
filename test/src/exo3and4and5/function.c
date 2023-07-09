#include "function.h"


extern uint8_t** list_to_matrice(uint32_t raw, uint32_t colums, uint8_t* buffer){

    struct pi_device cluster_dev;
    uint8_t **l1_array = (uint8_t **) pi_cl_l1_malloc(&cluster_dev, raw*sizeof(uint32_t*));

    for(uint32_t i = 0; i < raw ; i++){

        l1_array[i] = (uint8_t *) pi_cl_l1_malloc(&cluster_dev, colums*sizeof(uint32_t*));

        for(uint32_t j = 0; j < colums; j++) {

            l1_array[i][j] = buffer[i+j];
        }
    }
    return l1_array;
}



extern uint8_t* matrice_to_list(uint32_t raw, uint32_t colums, uint8_t** buffer){

    struct pi_device cluster_dev;
    uint8_t *l1_array = (uint8_t *) pi_cl_l1_malloc(&cluster_dev, raw*colums);
    uint32_t temp = 0; 
    for(uint32_t i = 0; i < raw;i++){

        for(uint32_t j = 0; j < colums; j++) {
            l1_array[temp] = buffer[i][j];
            temp++;
        }

    }

    return l1_array;
}

extern uint8_t** addMatrices(uint8_t** matrix1, uint8_t** matrix2, uint32_t raw, uint32_t colums) {
    
    struct pi_device cluster_dev;
    uint8_t** result = (uint8_t **) pi_cl_l1_malloc(&cluster_dev, raw*sizeof(uint32_t*));
    
    for (uint32_t i = 0; i < raw; i++) {

        result[i] = (uint8_t *) pi_cl_l1_malloc(&cluster_dev, colums*sizeof(uint32_t*));
        for (uint32_t j = 0; j < colums; j++) {
            result[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    
    return result;
}


/* We assume both are square Array */
extern uint8_t** multiplySquareMatrices(uint8_t** matrix1, uint8_t** matrix2, uint32_t size) {


    struct pi_device cluster_dev;
    uint8_t** result = (uint8_t **) pi_cl_l1_malloc(&cluster_dev, size*sizeof(uint32_t*));
    for (uint32_t i = 0; i < size; i++) {
        result[i] = (uint8_t *) pi_cl_l1_malloc(&cluster_dev, size*sizeof(uint32_t*));
    }
    
    // Perform matrix multiplication
    for (uint32_t i = 0; i < size; i++) {
        for (uint32_t j = 0; j < size; j++) {
            result[i][j] = 0;
            for (uint32_t k = 0; k < size; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    
    return result;
}

extern void printMatrix(int** matrix, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
