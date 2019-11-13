#!/bin/bash

if [ "$1" == "start" ]; then

    if [ -n "$MB_SPINNER_PID" ]; then
        return
    fi

    >&2 echo "Starting SPINNER..."
    # Start a process that runs as a keep-alive
    # to avoid travis quitting if there is no output
    (while true; do
        sleep 60
        >&2 echo "SPINNING..."
    done) &
    MB_SPINNER_PID=$!
    disown

fi

if [ "$1" == "stop" ]; then

    if [ ! -n "$MB_SPINNER_PID" ]; then
        return
    fi
    
    kill $MB_SPINNER_PID
    unset MB_SPINNER_PID

    >&2 echo "SPINNER stopped..."

fi

