#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>
#include <CL/cl.h>
#include <vector>


cl_program load_program(cl_context context, const char* filename)
{
	std::ifstream in(filename, std::ios_base::binary);
	if(!in.good())
	{
		return 0;
	}
	in.seekg(0, std::ios_base::end);
	size_t length = in.tellg();
	in.seekg(0, std::ios_base::beg);

	std::vector<char> data(length + 1);
	in.read(&data[0], length);
	data[length] = 0;

	const char* source = &data[0];
	cl_program program = clCreateProgramWithSource(context, 1, &source, 0, 0);
	if(program == 0)
	{
		return 0;
	}
	if(clBuildProgram(program, 0, 0, 0, 0, 0) != CL_SUCCESS)
	{
		return 0;
	}
	return program;
}



unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	unsigned int *img = image_data;
	unsigned int *ref_histogram_results;
	unsigned int *ptr;

	ref_histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	ptr = ref_histogram_results;
	memset (ref_histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));

	// histogram of R
	for (unsigned int i = 0; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of G
	ptr += 256;
	for (unsigned int i = 1; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of B
	ptr += 256;
	for (unsigned int i = 2; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	return ref_histogram_results;
}

int main(int argc, char const *argv[])
{
	cl_int status = 0;
	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if(status != CL_SUCCESS)
	{
		printf("Error: Getting Platforms\n");
		return EXIT_FAILURE;
	}
	cl_platform_id *platforms;
	if(numPlatforms > 0)
	{
		platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		if(status != CL_SUCCESS)
		{
			printf("Error: Getting Platform Ids.(clGetPlatformIDs\n)");
			return -1;
		}
	}
	
	for(unsigned int i = 0; i < numPlatforms; ++i)
	{
		char pbuff[100];
		status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pbuff), pbuff, NULL);
		platform = platforms[i];
		if(!strcmp(pbuff, "NVIDIA Corporation"))
		{
			break;
		}
	}

	
	cl_context_properties cps[3] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platform,
		0};
	cl_context_properties *cprops = (NULL == platform) ? NULL : cps;
	cl_context context = clCreateContextFromType(
		cprops,
		CL_DEVICE_TYPE_GPU,
		NULL,
		NULL,
		&status);
	if(status != CL_SUCCESS)
	{
		printf("ERROR: Creating Context.\n");
		return EXIT_FAILURE;
	}
	
	size_t deviceListSize;
	status = clGetContextInfo(context,
		CL_CONTEXT_DEVICES,
		0,
		NULL,
		&deviceListSize);
	if(status != CL_SUCCESS)
	{
		printf("Error: Getting Context Info device list size\n");
		return EXIT_FAILURE;
	}
	cl_device_id *devices = (cl_device_id *)malloc(deviceListSize);
	if(devices == 0)
	{
		printf("Error: No device found.");
		return EXIT_FAILURE;
	}
	
	status = clGetContextInfo(context,
		CL_CONTEXT_DEVICES,
		deviceListSize,
		devices,
		NULL);
	if(status != CL_SUCCESS)
	{
		printf("ERROR: Getting Context Info");
		return EXIT_FAILURE;
	}

	cl_command_queue queue = clCreateCommandQueue(context, devices[0], 0, 0);
	if(queue == 0)
	{
		printf("can't create command queue\n");
		clReleaseContext(context);
		return 0;
	}
	
	std::vector<int> histogram_results(256 * 3);
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("xxxxxx.out", std::ios_base::out);

	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}
	
	cl_mem cl_image = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(cl_int) * input_size,
		&image[0],
		NULL);
	cl_mem cl_results = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(cl_int) * 768,
		NULL,
		NULL);
	if(cl_image == 0 || cl_results == 0)
	{
		printf("Can't create buffer\n");
		clReleaseMemObject(cl_image);
		clReleaseMemObject(cl_results);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return 0;
	}

	cl_program program =load_program(context, "histogram.cl");
	if(program == 0)
	{
		printf("Can't load or build program\n");
		clReleaseMemObject(cl_image);
		clReleaseMemObject(cl_results);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return 0;
	}

	cl_kernel kernel_histogram = clCreateKernel(program, "histogram", 0);
	if(kernel_histogram == 0)
	{
		printf("Can't load kernel\n");
		clReleaseMemObject(cl_image);
		clReleaseMemObject(cl_results);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return 0;
	}
	
	clSetKernelArg(kernel_histogram, 0, sizeof(cl_mem), &cl_image);
	clSetKernelArg(kernel_histogram, 1, sizeof(cl_mem), &cl_results);
	size_t work_size = (size_t)input_size;

	
	
	int err = clEnqueueNDRangeKernel(queue, kernel_histogram, 1, 0, &work_size, 0, 0, 0, 0);
	
	if(err == CL_SUCCESS)
	{
		err = clEnqueueReadBuffer(queue, cl_results, CL_TRUE, 0, sizeof(unsigned int) * 256 * 3, &histogram_results[0], 0, 0, 0);
	}
	else
	{
		printf("error: clEnqueueNDRangeKernel");
	}
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
		{
			outFile << std::endl;
		}
		outFile << histogram_results[i]<< ' ';
	}

	//histogram_results = histogram(image, input_size);
	

	inFile.close();
	outFile.close();

	return 0;
}
