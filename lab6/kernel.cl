__kernel void convolution(const int filterWidth,
                          __constant float *filter,
                          const int imageHeight,
                          const int imageWidth,
                          __global const float *inputImage,
                          __global float *outputImage) 
{
    int idx = get_global_id(0);
    
    int halffilterSize = filterWidth / 2;
    float sum = 0; // Reset sum for new source pixel

    int i = idx / imageWidth;
    int j = idx % imageWidth;

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
    outputImage[idx] = sum;
}
