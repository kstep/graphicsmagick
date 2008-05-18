#!/bin/sh
find . -type f -print | egrep -v '(.#.*)|(CVS)|(core)|(~$)' | sed -e 's:\./::' | sort > MANIFEST
