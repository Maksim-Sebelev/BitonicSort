#ifndef TYPE 
#define TYPE int
#endif /* TYPE */

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 256
#endif /* LOCAL_SIZE */

__kernel void small_blocks_sizes(__global TYPE* data)
{
    /* assert( data.size >= LOCAL_SIZE) */

    uint it1 = get_local_id(0);
    uint git1 = get_global_id(0);
    uint git2;
    uint it2;
    uint block_size = 2;
    uint stage_comparing_distance;

    __local TYPE ldata[LOCAL_SIZE];

    ldata[it1] = data[git1];
    barrier(CLK_LOCAL_MEM_FENCE);
    
    /* small stages */
    for (/* block_size = 2 */; block_size <= LOCAL_SIZE; block_size <<= 1)
    {
        for (stage_comparing_distance = (block_size >> 1); stage_comparing_distance > 0; stage_comparing_distance >>= 1)
        {
            /*
                add 2^stage_comparing_distance by module 2^(stage_comparing_distance+1)
                thats mean get index of pair in current block, because current_block_size = 2^(stage_comparing_distance+1)
                current_block_size != block_size
            */
            it2 = it1 ^ stage_comparing_distance;
            if (it1 < it2)
            {
                TYPE a = ldata[it1];
                TYPE b = ldata[it2];

                if ((!(git1 & block_size)) == (a > b))
                {
                    ldata[it1] = b;
                    ldata[it2] = a;
                }
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
    }

    data[git1] = ldata[it1];
}

__kernel void big_block_sizes(__global TYPE* data, uint size)
{
    uint it1 = get_global_id(0);
    uint it2;
    uint block_size = 2 * LOCAL_SIZE;
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
