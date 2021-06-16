#include <iostream>
#include <cstdlib> /* 亂數相關函數 */
#include <ctime>   /* 時間相關函數 */
#include<pthread.h>
#include <time.h>
#include <random>
#include <omp.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

#define MAX 4294967295;


long long int total_sum = 0;
long long int total;
unsigned short nthread;
long long int sub_total[100];
pthread_mutex_t mutexID;

 // 傳入 thread 的參數型別
typedef struct
{
   int pid;
   int step;
} Arg;

int my_rand(unsigned int* seed){
    
    int val = ((*seed * 1103515245U) + 12345U) & 0x7fffffff;
    //cout << "val = " << val << endl;
    return val;
}

void *toss(void *para) {  
    
    Arg *thread_arg = (Arg*)para;    
    double x, y, dd;
    long long int temp_sum = 0;
    unsigned int seed = rand();
    //unsigned int seed = 1603977153;

    for (long long int i = thread_arg->pid; i < total; i+=nthread){
        x = (double) rand_r(&seed)/ RAND_MAX;
        y = (double) rand_r(&seed)/ RAND_MAX;
        if (x * x + y * y<= 1){
            temp_sum ++;
        }
    }
    sub_total[thread_arg->pid] = temp_sum;
   
/*
    for (long long int i = thread_arg->pid; i < total; i+=nthread){
        //x = (double) my_rand(&seed)/ RAND_MAX;
        //y = (double) my_rand(&seed)/ RAND_MAX;
        cout<<"x = "<<x<<endl;
        if (x * x + y * y<= 1){
            temp_sum ++;
        }
    }
    sub_total[thread_arg->pid] = temp_sum;
*/
    pthread_exit((void *)0);
    
}


int main(int argc, char* argv[]){
    nthread = atoi(argv[1]);
    total = atoi(argv[2]);
    pthread_t threadID[nthread];
    Arg thread_arg[nthread];
    long long int avg = total / nthread;
    pthread_mutex_init(&mutexID, NULL);
   // 設定 pthread 性質是要能 join
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    srand((unsigned)time(NULL));


    
    for (int i = 0; i < nthread; i++) {
        thread_arg[i].pid = i;
        thread_arg[i].step = avg;
        if( pthread_create(&threadID[i], &attr, toss, (void *)&thread_arg[i]) != 0){
            printf("create thread failed");
        }
    }
   // 回收性質設定
   pthread_attr_destroy(&attr);
   void *status;
    
    for(int i = 0; i < nthread; i++){
        pthread_join(threadID[i], NULL);
    }
    for(int i = 0; i < nthread; i++){
        total_sum += sub_total[i];
    }
    pthread_mutex_destroy(&mutexID);
    

    double pi = 4 * total_sum / ((double)total);
    cout<< pi << endl;
    pthread_exit(NULL);

    return 0;
}
