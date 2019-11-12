#!/usr/bin/env python3

import sys
from random import randint

def main(num):
    nums = []
    for i in range(num):
        nums.append(randint(1, num))

    print("%%")
    print("%%", file=sys.stderr)
    for n in zip(nums, sorted(nums)):
        print(n[0])
        print(n[1], file=sys.stderr)

if __name__ == '__main__':
    main(int(sys.argv[1]))
