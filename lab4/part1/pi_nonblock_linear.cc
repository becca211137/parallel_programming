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
    long long int temp_result = 0;
    long long int temp_array[world_size];
    temp_result = toss(iter,world_rank);
    int tag = 0;

    MPI_Request reqs[world_size-1];
    MPI_Status status[world_size-1];


    if (world_rank > 0)
    {
        // TODO: MPI workers
        MPI_Send(&temp_result, 1, MPI_LONG_LONG, 0, tag, MPI_COMM_WORLD);
        // printf("rank %d send, reqs = %d\n", world_rank, world_rank-1);
    }
    else if (world_rank == 0)
    {
        // printf("size = %d\n", world_size);
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        temp_array[0] = temp_result;
        long long temp[100];
        for(int i = 1; i < world_size; i++){
            MPI_Irecv(&temp_array[i], 1, MPI_LONG_LONG, i, tag, MPI_COMM_WORLD, &reqs[i-1]);
            // temp_result += temp;
        }
        MPI_Waitall(world_size-1, reqs, MPI_STATUS_IGNORE);
        // printf("before wait\n");
        
        // for(int i=0; i<(world_size-1)*2; i++)
        //     printf("reqs %d = %s\n", i, reqs[i]);
        // printf("after wait\n");
    }

    // MPI_Request r[world_size];
    // MPI_Status s[world_size];
    // if (world_rank > 0)
    // {
    //     // TODO: MPI workers
    //     MPI_Send(&temp_result, 1, MPI_LONG_LONG, 0, tag, MPI_COMM_WORLD);
    //     // for(int i = 1; i<world_size;i++)
    //     //     MPI_Wait(&r[i-1], &s[i-1]);
    //     // printf("rank %d send, reqs = %d\n", world_rank, world_rank-1);
    // }
    // else if (world_rank == 0)
    // {
    //     // printf("size = %d\n", world_size);
    //     // TODO: non-blocking MPI communication.
    //     // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
    //     temp_array[0] = temp_result;
    //     for(int j=1; j<world_size;j++)
    //         MPI_Irecv(&temp_array[j], 1, MPI_LONG_LONG, j, tag, MPI_COMM_WORLD, &r[j-1]);
        
    //     // printf("before wait\n");
    //     for(int i = 1; i<world_size;i++)
    //         MPI_Wait(&r[i-1], &s[i-1]);
    //     // printf("after wait\n");
    // }

    if (world_rank == 0)
    {
        // TODO: PI result
        for(int i=1; i<world_size; i++)
            temp_result += temp_array[i];
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
