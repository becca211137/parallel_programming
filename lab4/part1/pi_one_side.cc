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

int check(long long int *temp_array, int size){
    for(int i = 1; i < size; i++){
        if(temp_array[i]==0){
            return 0;
        } 
    }
    return 1;
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

    MPI_Win win;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    // TODO: MPI init
    long long int iter = tosses/world_size;
    long long int temp_result = 0;
    long long int temp;
    temp_result = toss(iter,world_rank);
    long long int result = 0;

    if (world_rank == 0)
    {
        long long int temp_array[world_size];
        MPI_Alloc_mem(world_size * sizeof(long long), MPI_INFO_NULL, &temp_array);
        // Master
        for(int i = 0; i < world_size; i++){
            temp_array[i] = 0;
        }
        temp_array[0] = temp_result;

        // Create a window. Set the displacement unit to sizeof(int) to simplify
        // the addressing at the originator processes
        MPI_Win_create(temp_array, world_size * sizeof(long long int), sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        int ready = 0;
        while(!ready){
          MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
          ready = check(temp_array, world_size);
          MPI_Win_unlock(0, win);
        }
        for(int i = 0; i < world_size; i++){
            result += temp_array[i];
        }
        MPI_Win_free(&win);
    }
    else
    {
       // Worker processes do not expose memory in the window
       MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
       // Register with the master
       MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
       MPI_Put(&temp_result, 1, MPI_LONG_LONG_INT, 0, world_rank, 1, MPI_LONG_LONG_INT, win);
       MPI_Win_unlock(0, win);
        MPI_Win_free(&win);
    }


    if (world_rank == 0)
    {
        pi_result = 4 * result / ((double)tosses);
        // TODO: handle PI result
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}