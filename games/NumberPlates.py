#!/usr/bin/python3

# Sample optparse implementation

import os
import sys
import random
import string
import easygui

from optparse import OptionParser

regional_transport_authority_names = {
        "LA" : "Ladakh",
        "JK" : "Jammu & Kashmir",
        "HP" : "Himachal Pradesh",
        "PB" : "Punjab",
        "HR" : "Haryana",
        "UA" : "Uttaranchal",
        "UK" : "Uttarakhand",
        "DL" : "Delhi",
        "RJ" : "Rajasthan",
        "UP" : "Uttar Pradesh",
        "BR" : "Bihar",
        "SK" : "Sikkim",
        "JH" : "Jharkhand",
        "AR" : "Arunachal Pradesh",
        "AS" : "Assam",
        "NL" : "Nagaland",
        "ML" : "Meghalaya",
        "MN" : "Manipur",
        "MZ" : "Mizoram",
        "TR" : "Tripura",
        "GJ" : "Gujarat",
        "MP" : "Madhya Pradesh",
        "CG" : "Chattisgarh",
        "WB" : "West Bengal",
        "MH" : "Maharashtra",
        "OR" : "Orissa",
        "OD" : "Odisha",
        "AP" : "Andhra Pradesh",
        "TS" : "Telangana",
        "GA" : "Goa",
        "KA" : "Karnataka",
        "KL" : "Kerala",
        "TN" : "Tamil Nadu",
        "DD" : "Daman & Diu",
        "DN" : "Dadar & Nagar Haweli",
        "PY" : "Pondicherry",
        "AN" : "Andaman & Nicobar Islands",
        "LD" : "Lakshadweep Islands",
        }

random.seed()
authorities=list(regional_transport_authority_names.keys())
letters = string.ascii_uppercase

def get_plate():
    auth=random.randrange(len(authorities))
    district=random.randrange(50)

    serie_len=random.randrange(1,3)
    serie=''.join(random.choice(letters) for i in range(serie_len))
    idx=random.randrange(10000)

    numberplate="  {}-{:02d}\n{} {}".format(authorities[auth],district,serie,idx)

    return [numberplate, regional_transport_authority_names[authorities[auth]]]

def build_parser():
    usagestr = "%prog [options] <program>"
    parser = OptionParser(usage=usagestr)

    parser.add_option(
        "-c", "--count", dest="count", action="store", default=25,
        help="""Count of questions to ask."""
        )
    parser.add_option(
        "-a", "--answers", dest="answers", action="store", default="Y",
        help="Show the answers or not.",
        )
    return parser

yes_answers= ['Y','Yes','yes']
no_answers= ['N','No','no']

def validate_opts(opts):
    if opts.count:
        try:
            int(opts.count)
        except ValueError:
            print("\nError: Value of the count option must be a number.")
            parser.print_help()
            sys.exit(4)
    if opts.answers:
        if opts.answers not in yes_answers+no_answers:
            print("\nError: Value of the answers option must be Y/Yes/yes/N/No/no .")
            parser.print_help()
            sys.exit(8)
if __name__ == '__main__':

    parser = build_parser()
    (opts, args) = parser.parse_args()
    validate_opts(opts)
    print("Showing {} questions (show answers:{})".format(opts.count,opts.answers))

    for x in range(int(opts.count)):
        result=get_plate()
        easygui.msgbox(result[0], 'Question')
        if opts.answers in yes_answers:
            easygui.msgbox(result[1], 'Answer')

# vim: set ts=4 sw=4 et:
