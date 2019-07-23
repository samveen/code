#!/usr/bin/perl

use strict;
use Data::Dumper;

sub factors($) {
    my($x)=@_;

    my $ret=[];

    for my $i (2..$x) {
        $ret->[$i]=0;
        while($x>1 and $x%$i == 0){
            $x/=$i;
            ++$ret->[$i];
        }
    }
    return $ret if $x == 1;
}

my $max=20;

my $maxF=[];

for my $i (2..$max) {
    $maxF->[$i]=0;
    my $t=factors($i);
    for my $j(2..$i) {
        $maxF->[$j]=$t->[$j] if $maxF->[$j] < $t->[$j];
    }
}

print Dumper($maxF);

my $lcm=1;

for my $i (2..$max) {
    $lcm*=$i**$maxF->[$i];
}

print $lcm,"\n";
