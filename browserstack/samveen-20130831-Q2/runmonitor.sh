#!/bin/bash

# Daemonization script

## Close terminal
exec </dev/null
exec >/dev/null
exec 2>/dev/null

# Complete daemonization by becoming the session leader
exec setsid bash monitor.sh "$@"
