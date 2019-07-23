#!/usr/bin/perl
use strict;

my $sum=0;
foreach my $i (1..999) {
    $sum+=$i if ($i%3==0 || $i%5==0);
}
print "Sum = $sum\n";

