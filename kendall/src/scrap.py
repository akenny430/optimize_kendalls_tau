import scipy.stats as st 
from scipy.stats._stats import _kendall_dis
import time 

import numpy as np 

# starting with two vectors 
x0: np.ndarray = np.array([12, 2, 1, 12, 2])
y0: np.ndarray = np.array([1, 4, 7, 1, 0])
# can always assume the once we pass them in, vectors will have same size and will not have any NaN values 


# at beginning of function, does this do anything? 
# for us, does not 
x = np.asarray(x0).ravel()
y = np.asarray(y0).ravel()
print(x)
print(y)

# need to have size saved 
size: int = x.size 

# first permutations, start with y 
print('\n') 
perm_index = np.argsort(y) 
x = x[perm_index] # x sorted in same way as y 
y = y[perm_index] # y is sorted on its own 
print(x)
print(y)
# then, goes through y, if prev value matches current value, keep it as the same, starting with 1 
# [0 1 1 4 7] -> [1 2 2 3 4]
y = np.r_[True, y[1:] != y[:-1]].cumsum(dtype=np.intp)
print(y) 

# then doing same with x 
print('\n') 
perm_index = np.argsort(x) 
x = x[perm_index] # x sorted in same way as y 
y = y[perm_index] # y is sorted on its own 
print(x) # now x is sorted on its own 
print(y) # y is sorted in terms of x now, but now y has its own indexes now 
# same idea with x, do cumulative sum or sorting 
# [ 1  2  2 12 12] -> [1 2 2 3 3]
x = np.r_[True, x[1:] != x[:-1]].cumsum(dtype=np.intp)
print(x) 
print(y) 

# now that we have these int vectors, have to find distance between them 
# at this point now, the x ones are sorted, and the y ones are not 
# ripped this from their repo, not sure how it works, but hopefully we can just copy stuff over 
"""
TODO: 
- what does >> do in Python 
- what does & do in python? its an integer? 

>> is bit shifting. bit shifting by 14 bits, even for large values that sets it to 0? 
& is bitwise and. 
have no idea why we are doing this, but thats fine, can just do it easily in c++ too. 
"""
def _my_kendall_distance(x: np.ndarray, y: np.ndarray) -> int: 
    sup: int = 1 + np.max(y) 
    # arr: np.ndarray = np.zeros(sup + ((sup - 1) >> 14), dtype=np.intp)
    arr: np.ndarray = np.zeros(sup, dtype=np.intp)
    # print(sup)
    # print(arr.size) # same deal here, shifting has no effect 
    i: int = 0 
    k: int = 0 
    size: int = x.size 
    idx: int # not sure what this is supposed to be 
    dis: int = 0 

    while i < size:
        while k < size and x[i] == x[k]:
            dis += i
            idx = y[k]
            while idx != 0:
                # dis -= arr[idx + (idx >> 14)]
                dis -= arr[idx]
                idx = idx & (idx - 1)

            k += 1

        while i < k:
            idx = y[i]
            while idx < sup:
                # arr[idx + (idx >> 14)] += 1
                arr[idx] += 1
                idx += idx & -idx
            i += 1

    return dis 

print('\n') 
# dis: int = _my_kendall_distance(x, y) 
# true_dis: int = _kendall_dis(x, y) 
dis: int = _kendall_dis(x, y) 
print(dis) 
# print(true_dis)  

# testing the two operations 
# for v in range(1_000_000, 0, -1): 
# for v in range(2_000, 0, -1): 
#     print(f"\nValue: {v}") 
#     print(f"Shifted right by 14 bits: {v >> 14}") # this seems to just always make it 0? why even bother doing anything then? 
#     print(f"Intersection with itself and minus 1: {v & (v - 1)}") 
#     print(f"{v:b}")
#     print(f"{(v - 1):b}")
#     print(f"{v & (v - 1):b}")
#     time.sleep(1)

# not sure what this is supposed to be 
obs = np.r_[True, (x[1:] != x[:-1]) | (y[1:] != y[:-1]), True]
print('\n')
print(obs) 

# print('\n') 
# print(x[1:])
# print(x[:-1])
# print((x[1:] != x[:-1])) # actually is array of length 4? 
# print((y[1:] != y[:-1]))
# print((x[1:] != x[:-1]) | (y[1:] != y[:-1]))

# difference of the nonzero (True) indices 
# [0, 1, 2, 3, 5] -> [1, 1, 1, 2] i.e. [ 1 - 0, 2 - 1, 3 - 2, 5 - 3 ] 
cnt = np.diff(np.nonzero(obs)[0]).astype('int64', copy=False)
print('\n') 
# print(np.nonzero(obs))
print(cnt) 

# then getting ties 
ntie = (cnt * (cnt - 1) // 2).sum()
print('\n') 
# these values are increasing, and can't be less than 1, so will never have a negative number 
# print(cnt * (cnt - 1))
# print(cnt * (cnt - 1) // 2)
print(ntie) 
# NOTE: can do "cnt" and "ntie" steps at once 

# getting the number of ties in x an dy 
# def count_rank_tie(ranks):
#     cnt = np.bincount(ranks).astype('int64', copy=False)
#     cnt = cnt[cnt > 1]
#     return ((cnt * (cnt - 1) // 2).sum(),
#             (cnt * (cnt - 1.) * (cnt - 2)).sum(),
#             (cnt * (cnt - 1.) * (2*cnt + 5)).sum())

# # their function also computes some stuff for stats, but we don't care about that 
# # so only focus on computing the relevant one 
def my_count_rank_tie(ranks) -> int:
    print('\n')
    print(ranks)
    cnt = np.bincount(ranks).astype('int64', copy=False)
    print(cnt) 
    cnt = cnt[cnt > 1]
    print(cnt) 
    return (cnt * (cnt - 1) // 2).sum() 

# # xtie, x0, x1 = count_rank_tie(x)
# # ytie, y0, y1 = count_rank_tie(y)
xtie = my_count_rank_tie(x)
ytie = my_count_rank_tie(y)
print('\n')
print(xtie)
print(ytie) 

# # size 
tot = size * (size - 1) // 2
print('\n')
print(tot)

# last step 
con_minus_dis = tot - xtie - ytie + ntie - 2 * dis
tau = con_minus_dis / np.sqrt(tot - xtie) / np.sqrt(tot - ytie)
true_tau = st.kendalltau(x0, y0)[0] 
print('\n')
print(tau)
print(true_tau)