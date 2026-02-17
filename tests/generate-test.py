from random import *
import sys

int_min = -2147483648
int_max =  2147483647

min_size = 200000
max_size = 800000

if len(sys.argv) >= 0:
    n = int(sys.argv[1])
else:
    n = randint(min_size, max_size)

for i in range(0, n + 1):
    print(n - i, end = ' ')
    # print(randint(int_min, int_max), end = ' ')


