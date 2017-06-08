#!/usr/bin/perl -w
# Copyright 2013 onwards, Samveen S. Gulati
# This code is provided under the HIRE ME/PAY ME License (Modified 2 Clause BSD
# License). See LICENSE file for details

use strict;
use warnings;

use lib qw(lib);

use Env;

use Data::Dumper;
use List::Uniq ":all";
use WordNet::QueryData;

# No buffered output
$|=1;

# Input file:
my $inf="teaser_input.txt";
open(my $fh, "<", $inf) || die ("Cannot open input file $inf: $!");

# Initialise WordNet
my $wnhome="WordNet/dict/";
print "Initializing wordnet DB: ";
my $wn = WordNet::QueryData->new($wnhome);
print "Complete\n";

# WordNet Operation we will use
my $a="hypes"; # Hyperonymy: super-subordinate relation
my $count_wn_calls=0;

my $index=0;
foreach my $l (<$fh>) {
    ++$index;
    chomp $l;
    last if $l =~ /^0/; #End of input

    my @words = sort split / /, $l;
    my %hyperonyms = ();

    ######
    foreach my $w (@words) {
        ++$count_wn_calls;
        foreach my $s ($wn->querySense ($w, $a)) {
            ++$count_wn_calls;
            foreach my $us ($wn->querySense ($s, $a)) {
                ++$count_wn_calls;
                foreach my $h ($wn->querySense ($us, $a)) {
                    $hyperonyms{$h} .= " ".$w if defined($hyperonyms{$h});
                    $hyperonyms{$h} = $w unless defined($hyperonyms{$h});
                }
            }
        }
    }

    my $found="";
    while("true") {
        my $wc=scalar @words;
        foreach my $k (keys %hyperonyms) {
            my $kc = scalar split / /, $hyperonyms{$k};
            if ($wc - $kc == 1) {
                $found=$k;
                last;
            }
        }
        last unless $found eq "";
        last if (defined($hyperonyms{"entity#n#1"}) and $wc == scalar split / /, $hyperonyms{"entity#n#1"}); # Every entity ancestor has converged to root element

        my @kys = keys %hyperonyms;
        foreach my $k (@kys) {
            ++$count_wn_calls;
            foreach my $h ($wn->querySense ($k, $a)) {
                if(defined($hyperonyms{$h})) {
                    $hyperonyms{$h} = join " ", uniq sort split / /, $hyperonyms{$h}." ".$hyperonyms{$k};
                } else {
                    $hyperonyms{$h} = $hyperonyms{$k};
                }
            }
        }
    }

    print "Case $index: ";
    if($found ne "") {
        foreach my $w(@words) {
            if($hyperonyms{$found} !~ /\b$w\b/) {
                print "$w\n";
                last
            }
        }
    } else {
        print "Invalid case: Unable to determine odd one out";
    }

    ######
}
print "Total WN Calls: $count_wn_calls\n";
close ($fh);

