#!/usr/bin/perl
use strict;

# Cache of powers of two: All tail ends are powers of 2
my @end;
map {$end[2**$_]=$_;} (1..22);

my ($max,$maxlen)=(0,0);

foreach my $start (3..1000000) {
    my ($n,$len)=($start,0);
    while($n!=1) {
        if(defined($end[$n])) { # Does it match a cached tail end
            $len=$len+$end[$n];
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
    $end[$start]=$len-1; # Cache the new tail end
}
print "$max:$maxlen\n";
print scalar @end;
print "\n";
