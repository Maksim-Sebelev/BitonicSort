#ifndef TYPE 
#define TYPE int
#endif /* TYPE */

__kernel void bitonic_sort_gpu(__global TYPE* data, uint size)
{
    uint it1 = get_global_id(0);
    uint it2;
    uint block_size = 2;
    uint compare_distance_bit_mask;

    for (; block_size <= size; block_size <<= 1)
    {
        for (compare_distance_bit_mask = (block_size >> 1); compare_distance_bit_mask > 0; compare_distance_bit_mask >>= 1)
        {
            it2 = it1 ^ compare_distance_bit_mask;
            if (it1 < it2)
            {
                TYPE a = data[it1];
                TYPE b = data[it2];

                if ((!(it1 & block_size)) == (a > b))
                {
                    data[it1] = b;
                    data[it2] = a;
                }
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
    }
}
