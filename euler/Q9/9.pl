#!/usr/bin/perl
use strict;

for my $a(1..333) {
    for my $c (334..998) {
        my $b=1000-$a-$c;
        if($a**2+$b**2==$c**2) {
            print $a*$b*$c;
            exit;
        }
    }
}
