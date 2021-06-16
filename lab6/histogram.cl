__kernel void histogram(__global const int* image, __global int* result)
{
	int idx = get_global_id(0);
	int loc = 256 * (idx % 3) + image[idx];
	atomic_add(&result[loc], 1);
	
	
}
