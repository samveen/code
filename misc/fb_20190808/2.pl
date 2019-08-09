#/usr/bin/env perl
#
# Given an list of ages, write a function find_all_friend_request() to find friend requests
#  sent as per the following rules: 
# - Yes friends requests to people who's age is older than (K/2+6) (#NOT SURE OF THIS CONDITION)
# - Yes friend requests on __I dont remeber this condition__
# - No friend requests to anyone older than you
# - No friend request to anyone below 100

use strict;
use warnings;

use Data::Dumper;

my @list= (300, 100, 200, 46, 75, 250, 400);

sub test_condition1_for($$){
    return 0;
}

sub test_condition2_for($$){
    return 0;
}

sub find_all_friend_requests(@) {
    my @ages=sort @_; # No requests to anyone older by sorting
    my $total_requests=0;
    for(my $i = scalar(@ages)-1; $i>=0; --$i) {
        for(my $j=$i-1;$j>=0; --$j) {
            last if($ages[$j] < 100); # No requests to anyone below 100
            if (test_condition1_for($ages[$i],$ages[$j])) { # test condition 1
                ++$total_requests;
            } elsif (test_confition2_for($ages[$i],$ages[$j])) { # test condition 2
                ++$total_requests;
            }
        }
    }
    return $total_requests;
}

print Dumper(\@list);
print find_all_friend_requests(@list)."\n";

# vim: ts=4 sw=4 et:
