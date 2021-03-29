#/usr/bin/env python3
""" Question: Given a string, does it have a permutation that is a palindrome?
    My answer: create a dictionary of letters vs it's count. More than a single
    odd count of letters means that a permutation that is a palindrome CANNOT 
    happen.
"""


def is_palindrome (s: str) -> bool:
    t={}
    for c in s:
        if c in t:
            t[c]=t[c]+1
        else:
            t[c]=1
    odd=0
    for k in t:
        if t[k]%2 == 1:
            odd=odd+1
        if odd > 1:
            return False
    return True

l = ["hello","abba","abbas"]

for i in l:
    if is_palindrome(i):
        print("yes")
    else:
        print("no")

