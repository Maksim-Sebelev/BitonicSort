from random import *

int_min = -2147483648
int_max =  2147483647

min_size = 10000000
max_size = 10000000

n = randint(min_size, max_size)

for i in range(0, n):
    print(f"{randint(int_min, int_max)} ", end = '')

