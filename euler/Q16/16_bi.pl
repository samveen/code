#!/usr/bin/perl
use strict;
use Math::BigInt;
use List::Util qw(reduce);
my $pow=1000;
print reduce {$a + $b} (split //, Math::BigInt->new(2)->bpow($pow)->bstr());
print "\n";
