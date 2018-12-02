#!/bin/bash

DELAY=3s

# Log start of monitor
logger -t "monitor.sh[$$]" -p syslog.info "Starting monitor process $$."

# Start child process in the background. The command is \$1 and the parmeters
# are the rest of the parameters.
"$@" &

# make a note of the PID
PID=$!
logger -t "monitor.sh[$$]" -p syslog.info "Started process \"$@\" with pid $PID".
COUNT=1

# Add a slight artificial delay in case the process bounces too ofter
sleep $DELAY

# Now that the process has started, we use "wait" to check for exit;
while wait; do
    # Wait exited, the process has terminated. Check if the process is truly dead.
    PS_COUNT=$(ps --no-headers --pid $PID| wc -l)
    
    # This test is actually redundent as wait will keep waiting until the child exits.
    while [[ $PS_COUNT -gt 0 ]]; do
        logger -t "monitor.sh[$$]" -p syslog.err "Process $PID not exited, yet wait exited. Manual check required."
        sleep 30s
        PS_COUNT=$(ps --no-headers --pid $PID| wc -l)
    done

    "$@" &
    PID=$!
    logger -t "monitor.sh[$$]" -p syslog.info "Process $PID ended. Restarted \"$@\" with $PID."
    ((++COUNT))

    # Slight artificial delay
    sleep $DELAY
    
done
__EOS
