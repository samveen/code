#!/usr/bin/env perl
#
# Question:
# Given an array, divide it into 2 such that the sums of the 2 subarrays are equal

use strict;
use warnings;

use Data::Dumper;

my @arr=(1,2,3,4,5,5,10);
print Dumper(\@arr);

my ($startindex,$endindex)=(0,scalar(@arr)-1);

my ($startsum, $endsum) = ($arr[$startindex], $arr[$endindex]);

while($startindex<$endindex) {
    if($startsum<$endsum) {
        ++$startindex;
        $startsum += $arr[$startindex];
    } else {
        --$endindex;
        $endsum += $arr[$endindex];
    }
    last if (($startindex+1)==$endindex);
}

if ($startsum!=$endsum) {
    print "Not possible to divide the array";
    exit(-1);
} else {
    print $startsum,":",$endsum," at indices [",$startindex," ",$endindex,"]\n";
    print Dumper([@arr[0..$startindex]]);
    print Dumper([@arr[$endindex..(scalar(@arr-1))]]);
}

# vim: et ts=4 sw=4:
