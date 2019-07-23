#!/usr/bin/perl
use strict;

use List::Util qw/reduce/;

sub sum(@) {
    return reduce { $a+$b; } map {length } @_;
}

my @ones=qw/one two three four five six seven eight nine/;
my @teens=qw/eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen/;
my @tens=qw/twenty thirty forty fifty sixty seventy eighty ninety/;

# one to hundred:
my $hundred = sum(@ones) + length("ten") + sum(@teens) + sum(@tens)*(1+scalar(@ones)) + sum(@ones)*(scalar(@tens));

# one to thousand
my $thousand = length("onethousand") + $hundred + sum((map {$_."hundredand";} @ones))*100 - length("and")*scalar(@ones) + $hundred*scalar(@ones);

print "$thousand\n";
