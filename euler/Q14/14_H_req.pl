#!/usr/bin/perl
use strict;

# Cache of powers of two: All tail ends are powers of 2
my %end = (map {2**$_ => $_} (1..40));

my ($max,$maxlen,$top,$topstart)=(0,0,0,0);

foreach my $start (800000..1000000) {
    my ($n,$len)=($start,0);
    my @stack=();
    while($n!=1) {
        ($topstart,$top)=($start,$n) if $n>$top;
        if(exists($end{$n})) { # Does it match a cached tail end
            $len=$len+$end{$n};
            $n=1;
        } else {
            push @stack, $n;
            if($n%2==1) {
                $n= 3*$n + 1;
            } else {
                $n=$n/2;
            }
        }
        ++$len;
    }
    ($max,$maxlen)=($start,$len) if $len > $maxlen;
    foreach my $tail (@stack) {
        $end{$tail}=--$len; # Cache the new tail end
    }
}
print "Maximum length $maxlen for sequence starting at $max\nCache size:", scalar keys %end;
print "\nMaximum cache key $top generated in seq $topstart\n";
