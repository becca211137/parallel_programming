#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

int my_rand(int* seed){
    
    int val = ((seed[0] * 1103515245U) + 12345U) & 0x7fffffff;
    seed[0] = val;
    //cout << "val = " << val << endl;
    return val;
}

long long int toss(long long int iter, int world_rank) {  
     
    double x, y, dd;
    long long int temp_sum = 0;
    int seed = world_rank;
    // seed = 1603977153;

    // for (long long int i = 0; i < iter; i++){
    //     x = (double) rand_r(&seed)/ RAND_MAX;
    //     y = (double) rand_r(&seed)/ RAND_MAX;
    //     if (x * x + y * y<= 1){
    //         temp_sum ++;
    //     }
    // }
    for (long long int i = 0; i < iter; i++){
        x = (double)my_rand(&seed)/ RAND_MAX;
        y = (double)my_rand(&seed)/ RAND_MAX;
        if (x * x + y * y<= 1){
            temp_sum ++;
        }
    }
    
    return temp_sum;
}
int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long int iter = tosses/world_size;
    long long int temp_result, temp;
    temp_result = toss(iter,world_rank);
    int tag=0;

    int cal = (int)log2(world_size);
    for(int i = 1; i <= cal; i++){
        // printf("iter = %d \n", i);
        if( (world_rank % (int)pow(2,(i-1))) == 0 && (world_rank/(int)pow(2,(i-1))) % 2 == 1){
            MPI_Send(&temp_result, 1, MPI_LONG_LONG, (world_rank-((int)pow(2,i-1))),i, MPI_COMM_WORLD);
            // printf("rank %d send to %d\n", world_rank, world_rank-((int)pow(2,i-1)));
        }
        if( (world_rank % ((int)pow(2,i) )) == 0){
            MPI_Recv(&temp, 1, MPI_LONG_LONG, (world_rank+((int)pow(2,i-1))), i, MPI_COMM_WORLD, &status);
            // printf("rank %d receive %d\n", world_rank, world_rank+((int)pow(2,i-1)));
            temp_result += temp;
        }
    }

    // TODO: binary tree redunction

    if (world_rank == 0)
    {
        // TODO: PI result        
        pi_result = 4 * temp_result / ((double)tosses);
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
