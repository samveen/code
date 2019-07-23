#!/usr/bin/perl
use strict;

# Cache of powers of two: All tail ends are powers of 2
my %end = (map {2**$_ => $_} (1..40));

my ($max,$maxlen,$top,$topstart)=(0,0,0,0);

foreach my $start (3..999999) {
    my ($n,$len)=($start,0);
    while($n!=1) {
        ($topstart,$top)=($start,$n) if $n>$top;
        if(exists($end{$n})) { # Does it match a cached tail end
            $len=$len+$end{$n};
            $n=1;
        } else {
            if($n%2==1) {
                $n= 3*$n + 1;
            } else {
                $n=$n/2;
            }
        }
        ++$len;
    }
    ($max,$maxlen)=($start,$len) if $len > $maxlen;
    $end{$start}=$len-1; # Cache the new tail end
}

print "$max:$maxlen\n";
print scalar keys %end;
print "Max: $top in seq $topstart\n";
