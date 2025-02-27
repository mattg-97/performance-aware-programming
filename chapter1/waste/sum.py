def SingleScalar(Count, Input):
    Sum = 0
    for Index in range(0, Count):
        Sum += Input[Index]
    return Sum

import time
import sys

CPUgHz = 4.2

print("Python: ", sys.version)
print("User supplied Hz: ", CPUgHz, "gHz")

TryCount = 100000
