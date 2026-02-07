#ifndef TYPE 
#define TYPE int
#endif

/*
this part of bitonic sort algorithm (betcher grid),
parallelize part of grid-comparing.
other iterations are made on cpu.
*/

__kernel void bitonic_sort_gpu_part(__global TYPE* data, uint size, uint block_size, uint compare_distance_bit_mask)
{
    uint it1 = get_global_id(0);

    uint it2 = it1 ^ compare_distance_bit_mask;

    if (it1 >= it2) return;

    TYPE a = data[it1];
    TYPE b = data[it2];

    if ((!(it1 & block_size)) != (a > b)) return;

    data[it1] = b;
    data[it2] = a;
}
