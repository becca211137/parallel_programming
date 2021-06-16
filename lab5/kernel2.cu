#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define THREAD_W 32
#define THREAD_H 32
#define BLOCK_W 50
#define BLOCK_H 38
using namespace std;

__device__ int mandel(float c_re, float c_im, int count)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < count; ++i)
  {

    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}
//float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations, stepX, stepY
__global__ void mandelKernel(
    int *ans, float lowerX, float lowerY,
    int resX, int resY, int maxIterations, float stepX, float stepY) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;
    
    // 根據 CUDA 模型，算出當下 thread 對應的 x 與 y
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    long long int idx = j * resX + i;
    float x = lowerX + i * stepX;
    float y = lowerY + j * stepY;
    ans[idx] = mandel(x, y, maxIterations);
    // printf("blockinx.x = %d, blockdim.x = %d, threadidx.x = %d\n", blockIdx.x ,blockDim.x , threadIdx.x);
    // printf("blockinx.y = %d, blockdim.y = %d, threadidx.y = %d\n", blockIdx.y ,blockDim.y , threadIdx.y);
    // printf("i = %d, j = %d, x = %f, y = %f, ans[%d] = %d\n", i,j,x,y,idx, ans[idx]);
}

// Host front-end function that allocates the memory and launches the GPU kernel
// hostFE(x1, y1, x0, y0, output, width = 1600, height = 1200, maxIterations);
void hostFE (
    float upperX, float upperY, float lowerX, float lowerY, 
    int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    dim3 blockSize(BLOCK_W, BLOCK_H);
    dim3 numBlock(THREAD_W, THREAD_H);
    int *ans_h, *ans_c;
    // cudaHostAlloc( (void**)&ans_h, resX * resY * sizeof( *ans_h ),cudaHostAllocDefault );
    cudaHostAlloc( (void**)&ans_h, resX * resY * sizeof(int),cudaHostAllocDefault );
    for(int i = 0; i<resX*resY; i++)
      ans_h[i] = 0;
    size_t pitch;
    // 宣告 Device (GPU) 記憶體
    cudaMallocPitch((void **)&ans_c, &pitch, resX * sizeof(int), resY);
    mandelKernel<<<blockSize, numBlock>>>(ans_c, lowerX, lowerY, resX, resY, maxIterations, stepX, stepY);

    // 等待 GPU 所有 thread 完成
    cudaDeviceSynchronize();
    
    // 將 Device 的資料傳回給 Host
    cudaMemcpy(img, ans_c, resX * resY * sizeof(int), cudaMemcpyDeviceToHost);
    // for(int i = 0; i < (resX * resY); i++)
    //     cout<<img[i]<<" ";
    

    cudaFree(ans_c);
    /*
    cout<<"width = "<<resX<<" height= "<<resY<<endl;
    int dev = 0;
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, dev);
    unsigned int maxThreads = deviceProp.maxThreadsPerBlock; //1024
    int *maxGrid = deviceProp.maxGridSize;
    cout<<maxGrid[0]<<endl;
    */
}
