import random
import math
import sys
import time

def time_taken(function, *args):
    start = time.time()
    function(*args)
    end = time.time()
    return end - start

def random_list(length = 20, limit = sys.maxsize):
    result = []
    for i in range(length):
        result.append(random.randint(0, limit))
    return result

def bad_quicksort(l):
    match l:
        case [] | [_]:
            return l
        case [x, y] if x <= y:
            return l
        case [x, y]:
            return [y, x]
        case [x, y, z] if x <= y <= z:
            return l
        case [x, y, z] if x >= y >= z:
            return [z, y, y]
        case [p, *rest]:
            a = bad_quicksort([x for x in rest if x <= p])
            b = bad_quicksort([x for x in rest if p < x])
            return a + [p] + b

def proper_quicksort(blep, low, high):
    if low >= 0 and high >= 0 and low < high:
        pivot_value = blep[math.floor((low + high) / 2)]
        l = low - 1
        r = high + 1
        while True:
            # python doesn't have do while apparently
            while True:
                l += 1
                if not blep[l] < pivot_value: break
            while True:
                r -= 1
                if not blep[r] > pivot_value: break
            if l >= r:
                new_pivot = r
                break
            tmp = blep[l]
            blep[l] = blep[r]
            blep[r] = tmp

        proper_quicksort(blep, low, new_pivot)
        proper_quicksort(blep, new_pivot + 1, high)
    return blep

def iterative_quicksort(blep, low, high):
    s = []
    s.append((low, high))
    # index 0 is low, index 1 is high
    while True:
        frame = s.pop()
        if frame[0] >= 0 and frame[1] >= 0 and frame[0] < frame[1]: 
            pivot_value = blep[math.floor((frame[0] + frame[1]) / 2)]
            l = frame[0] - 1
            r = frame[1] + 1
            while True:
                # python doesn't have do while apparently
                while True:
                    l += 1
                    if not blep[l] < pivot_value: break
                while True:
                    r -= 1
                    if not blep[r] > pivot_value: break
                if l >= r:
                    new_pivot = r
                    break
                tmp = blep[l]
                blep[l] = blep[r]
                blep[r] = tmp

            s.append((new_pivot + 1, frame[1]))
            s.append((frame[0], new_pivot))

        else:
            if len(s) == 0: return blep
