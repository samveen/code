#!/usr/bin/python

# Sample optparse implementation

import os
import sys
import time
import json
import shlex
import subprocess

from optparse import OptionParser

# Homebrew stupid_logger
def do_log(visible,message,newline=True): 
    if visible:
        if newline:
            print message
        else:
            print message,

# Options
def build_parser():
    usagestr = "%prog [options] <program>"
    parser = OptionParser(usage=usagestr)
    
    parser.add_option(
        "-w", "--wait", dest="wait", action="store", type="int", default=5,
        help="""Seconds to wait between attmpts to restart the service."""
        )

    parser.add_option(
        "-a", "--attempts", dest="attempts", action="store", type="int", default=3,
        help="Number of attempts before giving up.",
        )

    parser.add_option(
        "-c", "--check", dest="check", action="store", type="int", default=15,
        help="Check interval in seconds.",
        )
    
    parser.add_option(
        "-l", "--log", dest="log", action="store_true", default=False,
        help="Whether to generate logs of events.",
        )

    return parser

# check required arguments
def validate_args(args):
    """Validate our arguments and exit if we don't have what we want."""
    if not args:
        print "\nError: No arguments were specified.\n"
        parser.print_help()
        sys.exit(1)
    elif len(args) > 1:
        print "\nToo many arguments specified.\n"
        parser.print_help()
        sys.exit(2)
    
# check options
def validate_opts(opts):
    if opts.wait:
        try:
            int(opts.wait)
        except ValueError:
            print "\nError: Value of the wait option must be a number."
            parser.print_help()
            sys.exit(4)
    if opts.attempts:
        try:
            int(opts.attempts)
        except ValueError:
            print "\nError: Value of the wait option must be a number."
            parser.print_help()
            sys.exit(8)
    if opts.check:
        try:
            int(opts.check)
        except ValueError:
            print "\nError: Value of the wait option must be a number."
            parser.print_help()
            sys.exit(16)

# Do the monitor magic
if __name__ == '__main__':

    # build our parser and validate our args
    parser = build_parser()
    (opts, args) = parser.parse_args()
    validate_args(args)
    validate_opts(opts)

    do_log(opts.log,"monitor options: {0}".format(json.dumps(opts.__dict__)))

    # The process is a single value: split with shell lexer
    # If trying for arguments to the process, quote it all into one arg
    process = shlex.split(args[0]);
    do_log(opts.log,"Parse proc: {0}".format(json.dumps(process)))

    # initialize the retry counter
    retry_counter=0

    try:
        # get the ball rolling
        do_log(opts.log,"start proc")
        proc=subprocess.Popen(process)

        # sleep before check and retry
        do_log(opts.log,"sleep {0}s before next check".format(opts.check))
        time.sleep(int(opts.check))

        # monitor and restart
        while 1: # Dear Do-While, how I miss you!!
            # check status
            status = proc.poll()
            do_log(opts.log,"poll:",False)

            if status is None: # still running
                do_log(opts.log,"still running")
                retry_counter=0
            else: # done running - restart
                do_log(opts.log,"proc stopped with status code {0}".format(status))
                retry_counter=retry_counter+1

                if retry_counter <= opts.attempts:
                    do_log(opts.log,"retry {0}/{1}".format(retry_counter,opts.attempts))
                    proc=subprocess.Popen(process)
                    do_log(opts.log,"wait {0}s before retry".format(opts.wait))
                    time.sleep(int(opts.wait))
                else:
                    do_log(opts.log,"too many retries. Exiting")
                    sys.exit(32)

                continue

            # sleep before the next check
            do_log(opts.log,"sleep {0}s before next check".format(opts.check))
            time.sleep(int(opts.check))

    except KeyboardInterrupt:
        print "\nReceived interrupt via keyboard.  Shutting Down."
        sys.exit(0)

# vim: set ts=4 sw=4 et:
