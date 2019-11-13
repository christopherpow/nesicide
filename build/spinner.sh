#!/bin/bash

if [ "$1" == "start" ]; then

    if [ -f "spinning" ]; then
       exit -1;
    fi

    >&2 echo "Starting SPINNER..."
    # Start a process that runs as a keep-alive
    # to avoid travis quitting if there is no output
    (while true; do
        sleep 60
        >&2 echo "SPINNING..."
    done) &
    echo -n $! > spinning
    disown

fi

if [ "$1" == "stop" ]; then

    if [ ! -f "spinning" ]; then
       exit -1; 
    fi
    
    kill `cat spinning`
    rm spinning

    >&2 echo "SPINNER stopped..."

fi

