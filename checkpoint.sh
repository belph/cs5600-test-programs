#!/usr/bin/env bash
set -e

if [ "$#" -lt 2 ]; then
    >&2 echo "Usage: $0 time prog [args ...]"
    exit 1
fi

TIME=$1
shift

PROG=$(readlink $1 || echo $1)
shift
set -x

dmtcp_launch $PROG $@ &
sleep $TIME
dmtcp_command --checkpoint
kill -15 $!
