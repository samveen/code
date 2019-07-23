#!/usr/bin/perl
use strict;

use Data::Dumper;

my @primes = (2,3,5,7,11,13,17,19);
my $count=scalar @primes;

my $begin=19;

while($count<10001) {
    my $flag=0;
    $begin+=2;
    foreach (@primes){
        $flag=$begin%$_;
        last unless $flag;
    }
    if($flag!=0){
        push @primes, $begin;
        ++$count;
    }
}
print $begin ,"\n";
