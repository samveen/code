#!/usr/bin/python

from math import ceil, sqrt

number = 600851475143

count=0

div=3

while (number&1)==0:
    count=count+1
    number=number>>1
    count=count+1

if number==1:
    count=count+1
    number=2

root=ceil(sqrt(number))
count=count+1
while div<=root:
    count=count+1
    if(number%div==0):
        number=number/div
        count=count+1
        div=3
        root=ceil(sqrt(number))
        count=count+1
    count=count+1
    div+=2
    count=count+1

print "Largest prime factor =",number
count=count+1

print "Count:", count
