#!/usr/bin/perl
use strict;

my @pyramid = ( [qw/75/],
                [qw/95 64/],
                [qw/17 47 82/],
                [qw/18 35 87 10/],
                [qw/20 04 82 47 65/],
                [qw/19 01 23 75 03 34/],
                [qw/88 02 77 73 07 63 67/],
                [qw/99 65 04 28 06 16 70 92/],
                [qw/41 41 26 56 83 40 80 70 33/],
                [qw/41 48 72 33 47 32 37 16 94 29/],
                [qw/53 71 44 65 25 43 91 52 97 51 14/],
                [qw/70 11 33 28 77 73 17 78 39 68 17 57/],
                [qw/91 71 52 38 17 14 91 43 58 50 27 29 48/],
                [qw/63 66 04 68 89 53 67 30 73 16 69 87 40 31/],
                [qw/04 62 98 27 23 09 70 98 73 93 38 53 60 04 23/]
            );

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
