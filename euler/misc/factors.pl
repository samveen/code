#!/usr/bin/perl

use strict;
use Data::Dumper;

sub factors($) {
    my($x)=@_;

    for my $i (2..$x) {
        while($x>1 and $x%$i == 0){
            print $i,"\n";
            $x/=$i;
        }
    }
}

factors(1999993);
