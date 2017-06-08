Problem statement
-----------------

Given a set of words, can you find the odd one out?

Input
-----
Text file with following format:
* Each Line contains a set of words out of which one is an odd one out.
* Input termination is signalled by a line containing just `0`

Requirements
------------
* Perl Modules:
 - WordNet-QueryData-1.49 http://search.cpan.org/dist/WordNet-QueryData/QueryData.pm
 - List-Uniq-v0.21.0 http://search.cpan.org/dist/List-Uniq/lib/List/Uniq.pm
* Data Files
 - Princeton WordNet database files Version 3.0 or 3.1 https://wordnet.princeton.edu/wordnet/download/

Running
-------
* Install requirements (or place them into `lib/` which is added to `@INC`)
* Place the `dict` directory from the WordNet distribution into `WordNet/`
* Add or replace input word sets in `teaser_input.txt`
* Run `perl Teaser-CodeShowed.pl`
* Profit

NOTES
-----
* This is tested for just English language ASCII input.
* CodeShowed (कोड शॊड w) is a multilingual `punny` name used by my team for CPL2013

LICENSE
-------
This code is provided under the HIRE ME/PAY ME License (Modified 2 Clause BSD License). See LICENSE file for details
