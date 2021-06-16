#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    int filterSize = filterWidth * filterWidth;

    cl_command_queue queue = clCreateCommandQueue(*context, *device, 0, 0);
    if (queue == 0) {
        fprintf(stderr, "Can not create command queue\n");
        clReleaseProgram(*program);
        clReleaseContext(*context);
        return;
    }

    cl_kernel convolution = clCreateKernel(*program, "convolution", 0);
    if (convolution == 0) {
        fprintf(stderr, "Can not load kernel\n");
        clReleaseCommandQueue(queue);
        clReleaseProgram(*program);
        clReleaseContext(*context);
        return;
    }


    cl_mem cl_filter = clCreateBuffer(*context,
                                      CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                      sizeof(cl_float) * filterWidth * filterWidth,
                                      &filter[0], NULL);
    cl_mem cl_inputImage = clCreateBuffer(*context,
                                          CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                          sizeof(cl_float) * imageHeight * imageWidth,
                                          &inputImage[0], NULL);
    cl_mem cl_outputImage = clCreateBuffer(*context,
                                           CL_MEM_WRITE_ONLY,
                                           sizeof(cl_float) * imageHeight * imageWidth,
                                           NULL, NULL);

    clSetKernelArg(convolution, 0, sizeof(int), (void*)&filterWidth);
    clSetKernelArg(convolution, 1, sizeof(cl_mem), &cl_filter);
    clSetKernelArg(convolution, 2, sizeof(int), (void*)&imageHeight);
    clSetKernelArg(convolution, 3, sizeof(int), (void*)&imageWidth);
    clSetKernelArg(convolution, 4, sizeof(cl_mem), &cl_inputImage);
    clSetKernelArg(convolution, 5, sizeof(cl_mem), &cl_outputImage);

    size_t work_size = imageHeight * imageWidth;
    clEnqueueNDRangeKernel(queue, convolution, 1, 0, &work_size, 0, 0, 0, 0);
    clEnqueueReadBuffer(queue, cl_outputImage, CL_TRUE, 0, sizeof(float) * work_size,
                        &outputImage[0], 0, 0, 0);

    // clReleaseKernel(convolution);
    clReleaseProgram(*program);
    clReleaseMemObject(cl_filter);
    clReleaseMemObject(cl_inputImage);
    clReleaseMemObject(cl_outputImage);
    clReleaseCommandQueue(queue);
    clReleaseContext(*context);
}
