#!/usr/bin/perl
use strict;

use List::Util qw(reduce);
use POSIX;

use Data::Dumper;

sub factors ($) {
    my ($n)=@_;
    my @ret;
    my $r=ceil(sqrt($n));
    for(my $i=2;$i<=$r;++$i) {
        if($n%$i == 0) {
            push @ret, $i, $n/$i;
        }
    }
    return \@ret;
}

# Initialization
my $top=354;

my $number = reduce { $a+$b } 1..$top;
my $ret=factors($number);

# Search
while(498 > scalar @{$ret}) { # Check count
    ++$top;
    $number += $top;
    $ret=factors($number);
}

print "$top\t$number\t", scalar @{$ret};
print "\n";
