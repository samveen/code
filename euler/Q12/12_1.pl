#!/usr/bin/perl
use strict;

use List::Util qw(reduce);
use POSIX;

my @primes;

sub factors ($) {
    my ($n)=@_;
    my $ret={};
    my $r=ceil(sqrt($n));
    foreach my $i (@primes) {
        last if $i>$r;
        while($n>1 && $n%$i == 0) {
            if (defined $ret->{$i}){
                ++$ret->{$i} 
            } else {
                $ret->{$i}=1;
            }
            $n/=$i;
        }
    }
    return reduce {$a * $b} map {$_+1} values %{$ret};
}

# Initialization
# Start from K where K*(K+1)/2 >= 250^2 : solving this gives K = 354
my $top=354;
my $tri_number = reduce { $a+$b } 1..$top;

# Fill out prime array up with all primes up to sqrt(2^32) as
# our check goes up to sqrt(X) for any X and we're working with
# 32 bit numbers so max(X) is 2^32-1
@primes= split / /, qx#../misc/sieve2 65536#;

my $ret=factors($tri_number);

while(500 > $ret) {
    ++$top;
    $tri_number += $top;
    $ret=factors($tri_number);
}

print "$top\t$tri_number\t$ret\n";
