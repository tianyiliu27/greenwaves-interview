#include "pmsis.h"

/* Variables used. */

#define ARRAY_COLUM 64
#define ARRAY_ROW   64

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

PI_L2 static struct cl_args_s cl_arg;
uint8_t *res_add_check;
uint8_t *res_mult_check;

uint8_t** list_to_matrice(uint32_t raw, uint32_t colums, uint8_t* buffer){

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


uint8_t* matrice_to_list(uint32_t raw, uint32_t colums, uint8_t** buffer){

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

uint8_t** addMatrices(uint8_t** matrix1, uint8_t** matrix2, uint32_t raw, uint32_t colums) {
    
    struct pi_device cluster_dev;
    uint8_t** result = (uint8_t **) pi_cl_l1_malloc(&cluster_dev, raw);
    
    for (uint32_t i = 0; i < raw; i++) {

        result[i] = (uint8_t *) pi_cl_l1_malloc(&cluster_dev, colums);
        for (uint32_t j = 0; j < colums; j++) {
            result[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    
    return result;
}


/* We assume both are square Array */
uint8_t** multiplySquareMatrices(uint8_t** matrix1, uint8_t** matrix2, uint32_t size) {


    struct pi_device cluster_dev;
    uint8_t** result = (uint8_t **) pi_cl_l1_malloc(&cluster_dev, size);
    for (uint32_t i = 0; i < size; i++) {
        result[i] = (uint8_t *) pi_cl_l1_malloc(&cluster_dev, size);
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

void printMatrix(uint8_t** matrix, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        for (uint32_t j = 0; j < size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
/* Task executed by cluster cores. */
void cluster_dma(void *arg)
{
    struct cl_args_s *dma_args = (struct cl_args_s *) arg;
    uint8_t *l1_buffer = dma_args->l1_buffer;
    uint8_t *l1_buffer2 = dma_args->l1_buffer2;

    uint8_t *l2_in = dma_args->l2_in;
    uint8_t *l2_in2 = dma_args->l2_in2;
    
    uint8_t *l2_out = dma_args->l2_out;
    uint8_t *l2_out2 = dma_args->l2_out2;

    uint32_t buffer_size = dma_args->size;
    uint32_t coreid = pi_core_id(), start = 0, end = 0;

    struct pi_device cluster_dev;
    /*Expected Results to Check */



    /* Core 0 of cluster initiates DMA transfer from L2 to L1. */
    if (!coreid)
    {
        printf("Core %d requesting DMA transfer from l2_in to l1_buffer.\n", coreid);
        pi_cl_dma_copy_t copy;
        copy.dir = PI_CL_DMA_DIR_EXT2LOC;
        copy.merge = 0;
        copy.size = buffer_size;
        copy.id = 0;
        copy.ext = (uint32_t) l2_in;
        copy.loc = (uint32_t) l1_buffer;
        pi_cl_dma_memcpy(&copy);
        pi_cl_dma_wait(&copy);

        pi_cl_dma_copy_t copy2;
        copy2.dir = PI_CL_DMA_DIR_EXT2LOC;
        copy2.merge = 0;
        copy2.size = buffer_size;
        copy2.id = 0;
        copy2.ext = (uint32_t) l2_in2;
        copy2.loc = (uint32_t) l1_buffer2;
        pi_cl_dma_memcpy(&copy2);
        pi_cl_dma_wait(&copy2); 
        printf("Core %d : Transfer done.\n", coreid);

        /* Free l2_in and l2_in2 Memory */
        pi_l2_free(l2_in, buffer_size);
        pi_l2_free(l2_in2, buffer_size);

        uint32_t errors =0;

        /* Verification. */
        for (uint32_t i=0; i<buffer_size; i++)
        {
            if (l1_buffer[i] != (uint8_t) i)
            {
                errors++;
                printf("%d eme element = %d \n", i, (uint8_t) l1_buffer[i]);
            }

            if(l1_buffer2[i] != (uint8_t) 3){
                errors++;
                printf("%d eme element = %d \n", i, (uint8_t) l1_buffer2[i]);
            }

        }
        printf("\n");
        printf("************** Verification Transfer L2 to L1 ************** \n\r");
        if(errors == 0){
            printf(" L2 to L1 success ! \n");
            printf("\n");
        }

    }

    /* Barrier synchronisation before starting to compute. */
    pi_cl_team_barrier(0);

    /* Barrier synchronisation to wait for all cores. */
    pi_cl_team_barrier(0);

    /* Core 0 of cluster initiates DMA transfer from L1 to L2. */
    if (!coreid)
    {
        printf("Core %d requesting DMA transfer from l1_buffer to l2_out.\n", coreid);

        /* Transform into a 2D array located in L1 : */
        uint8_t** l1_buffer2D = list_to_matrice(ARRAY_ROW,ARRAY_COLUM,l1_buffer);
        uint8_t** l2_buffer2D = list_to_matrice(ARRAY_ROW,ARRAY_COLUM,l1_buffer2);

        /* Free Unused Memory */
        pi_cl_l1_free(&cluster_dev, l1_buffer, buffer_size);
        pi_cl_l1_free(&cluster_dev, l1_buffer2, buffer_size);
        

        /* Operation in L1 : */
        uint8_t** res_add = addMatrices(l1_buffer2D,l2_buffer2D,ARRAY_ROW,ARRAY_COLUM);
        uint8_t** res_mult = multiplySquareMatrices(l1_buffer2D,l2_buffer2D,ARRAY_ROW);
        // printMatrix(res_add,ARRAY_ROW);

        /*2D array to 1D array located in L1*/
        uint8_t* l3_add = matrice_to_list(ARRAY_ROW,ARRAY_COLUM,res_add);
        uint8_t* l3_mult = matrice_to_list(ARRAY_ROW,ARRAY_COLUM,res_mult);

        res_add_check = l3_add;
        res_mult_check = l3_mult;

        /* Free Unused Memory */
        pi_cl_l1_free(&cluster_dev, res_add, buffer_size);
        pi_cl_l1_free(&cluster_dev, res_mult, buffer_size);

        pi_cl_dma_copy_t copy;
        copy.dir = PI_CL_DMA_DIR_LOC2EXT;
        copy.merge = 0;
        copy.size = buffer_size;
        copy.id = 0;
        copy.ext = (uint32_t) l2_out;
        copy.loc = (uint32_t) l3_add;

        pi_cl_dma_memcpy(&copy);
        pi_cl_dma_wait(&copy);

        pi_cl_dma_copy_t copy1;
        copy1.dir = PI_CL_DMA_DIR_LOC2EXT;
        copy1.merge = 0;
        copy1.size = buffer_size;
        copy1.id = 0;
        copy1.ext = (uint32_t) l2_out2;
        copy1.loc = (uint32_t) l3_mult;

        pi_cl_dma_memcpy(&copy1);
        pi_cl_dma_wait(&copy1);

        printf("Core %d : Transfer done.\n", coreid);
    }
}

/* Cluster main entry, executed by core 0. */
void master_entry(void *arg)
{
    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    // pi_cl_team_fork(pi_cl_cluster_nb_pe_cores(), cluster_dma, arg);
    pi_cl_team_fork(1, cluster_dma, arg);
    printf("Cluster master core exit\n");
}

void test_cluster_dma(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;

    uint32_t nb_cl_pe_cores = pi_cl_cluster_nb_pe_cores();

    uint32_t buffer_size = (uint32_t) ARRAY_COLUM*ARRAY_ROW;
    uint8_t *l2_in = pi_l2_malloc(buffer_size);
    if (l2_in == NULL)
    {
        printf("l2_in buffer alloc failed !\n");
        pmsis_exit(-1);
    }

    uint8_t *l2_in2 = pi_l2_malloc(buffer_size);
    if (l2_in2 == NULL)
    {
        printf("l2_in buffer alloc failed !\n");
        pmsis_exit(-1);
    }

    uint8_t *l2_out = pi_l2_malloc(buffer_size);
    if (l2_out == NULL)
    {
        printf("l2_out buffer alloc failed !\n");
        pmsis_exit(-2);
    }

    uint8_t *l2_out2 = pi_l2_malloc(buffer_size);
    if (l2_out2 == NULL)
    {
        printf("l2_out buffer alloc failed !\n");
        pmsis_exit(-2);
    }

    /* L2 Array Init. */
    for (uint32_t i=0; i<buffer_size; i++)
    {
        l2_in[i] = i;
        l2_in2[i] = 3;
        l2_out[i] = 0;
        l2_out2[i] = 0;
    }

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-3);
    }

    uint8_t *l1_buffer = pi_cl_l1_malloc(&cluster_dev, buffer_size);
    if (l1_buffer == NULL)
    {
        printf("l1_buffer alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-4);
    }

    uint8_t *l1_buffer2 = pi_cl_l1_malloc(&cluster_dev, buffer_size);
    if (l1_buffer2 == NULL)
    {
        printf("l1_buffer alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-4);
    }

    /* Init arg struct. */
    cl_arg.size = buffer_size;
    cl_arg.l1_buffer = l1_buffer;
    cl_arg.l1_buffer2 = l1_buffer2;

    cl_arg.l2_in = l2_in;
    cl_arg.l2_in2 = l2_in2;
    cl_arg.l2_out = l2_out;
    cl_arg.l2_out2 = l2_out2;


    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task *task = pi_l2_malloc(sizeof(struct pi_cluster_task));
    if (task == NULL)
    {
        printf("Cluster task alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-5);
    }
    
    pi_cluster_task(task, master_entry, &cl_arg);

    printf("Sending task.\n");
    #if defined(ASYNC)
    pi_task_t wait_task;
    pi_task_block(&wait_task);
    pi_cluster_send_task_to_cl_async(&cluster_dev, task, &wait_task);
    printf("Wait end of cluster task\n");
    pi_task_wait_on(&wait_task);
    printf("End of cluster task\n");
    #else
    pi_cluster_send_task_to_cl(&cluster_dev, task);
    #endif  /* ASYNC */
    

    /* Verification L1 to L2  */  
    for(uint32_t i = 0; i < ARRAY_ROW*ARRAY_COLUM; i++){
        if(l2_out[i] != res_add_check[i]){
            errors ++;
        }
    }
    for(uint32_t i = 0; i < ARRAY_ROW*ARRAY_COLUM; i++){
        if(l2_out2[i] != res_mult_check[i]){
            errors ++;
        }
    }
    printf("\n");
    printf("************** Verification Transfer L2 to L1 ************** \n\r");
    if(errors == 0){
        printf(" L1 to L2 success ! \n");
        printf("\n");
    }


    pi_l2_free(task, sizeof(struct pi_cluster_task));
    pi_cl_l1_free(&cluster_dev, l1_buffer, buffer_size);

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    pi_l2_free(l2_out, buffer_size);
    pi_l2_free(l2_out2, buffer_size);
    pi_l2_free(l2_in, buffer_size);
    pi_l2_free(l2_in2, buffer_size);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Cluster DMA Test ***\n\n");
    return pmsis_kickoff((void *) test_cluster_dma);
}
