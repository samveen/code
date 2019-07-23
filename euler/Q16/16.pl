#!/usr/bin/perl
use strict;
use List::Util qw(reduce);
my $pow=1000;
print reduce {$a + $b} (split //, qx#echo \$(echo '2^1000' |bc)| sed 's/\\ //g'#);
print "\n";
