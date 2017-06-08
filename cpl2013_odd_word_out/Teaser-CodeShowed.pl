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
my $count_wn_calls_total=0;

my $index=0;
foreach my $l (<$fh>) {
    my $count_wn_calls=0;
    ++$index;
    chomp $l;
    last if $l =~ /^0/; #End of input

    my @words = sort split / /, $l;
    my %hypernyms = ();

    ######
    foreach my $w (@words) {
        ++$count_wn_calls;
        foreach my $s ($wn->querySense ($w, $a)) {
            ++$count_wn_calls;
            foreach my $us ($wn->querySense ($s, $a)) {
                ++$count_wn_calls;
                foreach my $h ($wn->querySense ($us, $a)) {
                    if(defined($hypernyms{$h})){
                        $hypernyms{$h}->{words} = join " ", uniq sort split / /, $hypernyms{$h}->{words}." ".$w;
                    } else {
                        $hypernyms{$h} = {};
                        $hypernyms{$h}->{words} = $w;
                    }
                }
            }
        }
    }

    my $found="";
    my $wc=scalar @words;
    while("true") {
        foreach my $k (keys %hypernyms) {
            my $kc = scalar split / /, $hypernyms{$k}->{words};
            if ($wc - $kc == 1) {
                $found=$k;
                last;
            }
        }
        last unless $found eq "";
        last if (defined($hypernyms{"entity#n#1"}) and $wc == scalar split / /, $hypernyms{"entity#n#1"}->{words}); # Every entity ancestor has converged to root element

        my @kys = keys %hypernyms;
        foreach my $k (@kys) {
            unless(exists($hypernyms{$k}->{parents})) { # Do this only if we dont have parents for the this key
                ++$count_wn_calls;
                my @parents=$wn->querySense ($k, $a);
                $hypernyms{$k}->{parents} = join " ", @parents; 
                #print Dumper $hypernyms{$k};
                foreach my $h (@parents) {
                    if(defined($hypernyms{$h})) {
                        $hypernyms{$h}->{words} = join " ", uniq sort split / /, $hypernyms{$h}->{words}." ".$hypernyms{$k}->{words};
                        if(defined $hypernyms{$h}->{parents}) {
                            my @ancestors=split / /, $hypernyms{$h}->{parents};
                            foreach $a (@ancestors) {
                                $hypernyms{$a}->{words} = join " ", uniq sort split / /, $hypernyms{$a}->{words}." ".$hypernyms{$k}->{words};
                                push @ancestors, split / /, $hypernyms{$a}->{parents} if defined($hypernyms{$a}->{parents});
                            }
                        }
                    } else {
                        $hypernyms{$h} = {};
                        $hypernyms{$h}->{words} = $hypernyms{$k}->{words};
                    }
                }
            }
        }
    }
    $count_wn_calls_total+=$count_wn_calls;
    print "Case $index(with $count_wn_calls WN Calls): ";
    if($found ne "") {
        foreach my $w(@words) {
            if($hypernyms{$found}->{words} !~ /\b$w\b/) {
                print "$w\n";
                last
            }
        }
    } else {
        print "Invalid case: Unable to determine odd one out";
    }
    #print Dumper \%hypernyms;

    ######
}
print "Total WN Calls: $count_wn_calls_total\n";
close ($fh);
