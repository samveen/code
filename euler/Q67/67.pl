#!/usr/bin/perl
use strict;

my @pyramid = ();
my $cache=[];

sub getcache($$) {
    my ($i,$j) = @_;
    return($cache->[$i][$j]);
}
sub setcache($$$){
    my ($i,$j,$v) = @_;
    $cache->[$i][$j]=$v;
}

sub leftchild($$) {
    my ($i,$j) = @_;
    return ($i+1,$j);
}

sub rightchild($$) {
    my ($i,$j) = @_;
    return ($i+1,$j+1);
}

sub value($$) {
    my ($i,$j) = @_;
    return($pyramid[$i][$j]);
}

sub maxpath($$) {
    my ($i,$j) = @_;

    return getcache($i,$j) if getcache($i,$j) > -1;
       
    my $lc = maxpath(leftchild($i,$j));
    my $rc = maxpath(rightchild($i,$j));

    if ( $lc > $rc ) {
        setcache($i,$j, (value($i,$j) + $lc));
        return (value($i,$j)+ $lc );
    } else {
        setcache($i,$j, (value($i,$j) + $rc));
        return ( value($i,$j)+ $rc );
    }
}

# Load input
open(my $fh, "<", "input_67.txt") || die ("Cannot open file: $!");

my @raw = <$fh>;

for(my $i=0; $i<scalar(@raw); ++$i) {
    chomp $raw[$i];
    $pyramid[$i]=[split / /, $raw[$i]];
}

# Set all cache values to -1
for(my $i=0;$i<scalar(@pyramid);++$i) {
    for(my $j=0;$j<scalar(@{$pyramid[$i]});++$j) {
        setcache($i,$j,-1);
    }
}

# set cache values of non existant children to 0.
# This is a hack to reduce conditional code in maxpath()
for(my $j=0;$j<=scalar(@{$pyramid[scalar(@pyramid)-1]});++$j) {
    setcache(scalar(@pyramid),$j,0);
}

print maxpath(0,0);
