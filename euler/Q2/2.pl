#!/usr/bin/perl
use strict;

sub LIMIT () { 4000000; }

my $sum=0;

my ($n1,$n2,$n3)=(0,1,0);

while ($n2 <= LIMIT) {
    $sum+=$n2 if (($n2 & 1)==0);
    $n3=$n2+$n1;
    $n1=$n2;
    $n2=$n3;
}
print "Sum = $sum\n";

