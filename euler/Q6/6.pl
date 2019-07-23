#!/usr/bin/perl
use strict;

my $max=100;
my($sum,$sqr_sum)=(0,0);

for(1..$max) {
    $sum+=$_;
    $sqr_sum+=$_*$_;
}

my $diff=($sum*$sum-$sqr_sum);

print $diff,"\n";

