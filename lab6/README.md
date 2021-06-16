# Assignment VI: OpenCL Programming
###### tags: `Parallel Programming`

#### Q1 (5 points): Explain your implementation. How do you optimize the performance of convolution?
在`hostFE.c`裡，分別先用`clCreateCommandQueue`及`clCreateKernel`做初始的設置，再來用`clCreateBuffer`將filter、imputImage，outputImage在 OpenCL 裝置上配置記憶體，最後用`clSetKernelArg`、`clEnqueueNDRangeKernel`、`clEnqueueReadBuffer`執行 OpenCL kernel，等計算完結果後，用`clRelease*`釋放掉相關記憶體
而在`kernel.cl`中則是根據`get_global_id`去分配計算的位置，
```c=
int idx = get_global_id(0);
int i = idx / imageWidth;
int j = idx % imageWidth;
```
而大致的convolution的方法則像serial版本的一樣，先對邊界進行判斷，如果沒有超出邊界就進行計算。
```c=
// Apply the filter to the neighborhood
for (int k = -halffilterSize; k <= halffilterSize; k++)
{
    for (int l = -halffilterSize; l <= halffilterSize; l++)
    {
        if (i + k >= 0 && i + k < imageHeight &&
            j + l >= 0 && j + l < imageWidth)
        {
            sum += inputImage[(i + k) * imageWidth + j + l] *
                   filter[(k + halffilterSize) * filterWidth +
                          l + halffilterSize];
        }
    }
}
```
