#!/usr/bin/python

LIMIT = 4000000

sum = 0
n1 = 0
n2 = 1
n3 = 0

while n2 < LIMIT:
    if ((n2&1)==0):
        sum=sum+n2
    n3=n2+n1
    n1=n2
    n2=n3

print "Sum =",sum
