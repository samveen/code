#!/usr/bin/python

primes = [2,3,5,7,11,13,17,19];
count= len(primes);

begin=19;

while count<10001:
    flag=0
    begin=begin+2
    for i in primes:
        flag=begin%i
        if flag == 0:
            break
    if flag!=0:
        primes.append(begin)
        count=count+1
print begin
