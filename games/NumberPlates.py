#!/usr/bin/python

# Sample optparse implementation

import os
import sys
import random
import string
import easygui

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

if __name__ == '__main__':

    while True:
        result=get_plate()
        easygui.msgbox(result[0], 'Question')
        easygui.msgbox(result[1], 'Answer')

# vim: set ts=4 sw=4 et:
