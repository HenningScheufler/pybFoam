"""calc.py located in the working directory"""

import numpy as np

test = np.array([1,2,3,4,5])
print("test-numpy-array:",test)


def add(i, j):
    print(i+j)
    return i + j


class Testing:
    def __init__(self,greet) -> None:
        print(greet)


