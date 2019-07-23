#!/usr/bin/perl
use strict;
use Data::Dumper;

my ($result,$answer,$count)=(0,0,0);

my $x=999;
my $y=$x-1;

my ($m_i,$m_j);

for(my $i=$x; $i>101; --$i) {
    for(my $j=$i; $j>=101; --$j) {
        $result=$i*$j;
        my $rev = reverse($result);
        ++$count;
        if ($result eq $rev) {
            if ($result > $answer) {
                $answer=$result;
                $m_i=$i;
                $m_j=$j;
            }
        }
    }
}
print "$answer ($m_i * $m_j)\n";
