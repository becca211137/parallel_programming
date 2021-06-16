#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

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
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long int iter = tosses/world_size;
    long long int send;
    long long int *recv;
    send = toss(iter, world_rank);
    if(world_rank == 0){
        recv = (long long int*)malloc(sizeof(long long int) * world_size);
    }

    // TODO: use MPI_Gather
    MPI_Gather(&send, 1, MPI_LONG_LONG, recv, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        long long int temp = 0;
        // TODO: PI result
        for(int i=0;i<world_size; i++){
            temp += recv[i];
        }
        pi_result = 4*temp / ((double)tosses);

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}
