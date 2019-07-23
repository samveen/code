#!/usr/bin/perl
use strict;

my @lattice=( [2] );

my $max=20;

for(my $size=1; $size<$max; ++$size) {
    # Fill the Lattice corners
    $lattice[$size][0]=$lattice[$size-1][0]+1;
    for(my $i=1; $i<$size; ++$i) {
        $lattice[$size][$i]=$lattice[$size][$i-1]+$lattice[$size-1][$i];
    }
    $lattice[$size][$size]=$lattice[$size][$size-1]*2;
}

print $lattice[$max-1][$max-1],"\n";

foreach my $row (@lattice) {
    map {print "$_ ";} @{$row};
    print "\n";
}
