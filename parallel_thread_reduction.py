# REDUCTION USING THE STRIDE STRAT

from typing import List, Optional

# vals = [1, 2, 3, 4, ... 32], alawyas a length of a power of 2
#   at each step, thread at index i reads value from thread i + stride
#   stride starts at len(vals) // 2
# return [[thread vals after red0], [vals after red1], ...]

def warp_reduce(values: List[Optional[int]]) -> List[List[Optional[int]]]:
    # Your code here
    ans = []
    current_vals = list(values)     # don't want to mutate input
    n = len(values)
    stride = n // 2

    while stride >= 1:
        for i in range(stride):
            # represents a single thread
            old = current_vals[i]
            partner = current_vals[i + stride]      # 1 thread pointsto the two points in the array

            # 1 thread does the reduction in place
            if old is None:
                current_vals[i] = None
            elif partner is None:
                current_vals[i] = old
            else:
                summed_val = current_vals[i] + current_vals[i + stride]
                current_vals[i] = summed_val
        
        # save a snapshot of what array looks like at this point
        ans.append(list(current_vals))
        stride //= 2


    return ans


# IN A CUDA KERNEL:
#   Inefficient to have to write from reg -> smem, only to read it back into registers, so this is a special instrinsic/function that reduces between registers quickly
#       mask = bitmask for the threads participating in this call
#       var = register variable to share
#       delta = stride, asking for value of var held by thread (my_id + delta)
#       width = "scope" of reduction : 32 -> 1, or 32 -> 4 x 8, ...
#   int __shfl_down_sync(unsigned mask, int var, unsigned int delta, int width=warpSize);