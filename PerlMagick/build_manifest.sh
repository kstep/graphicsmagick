#!/bin/sh
find . -type f -print | egrep -v '(.#.*)|(CVS)|(core)|(~$)|(.*\.cvsignore)|(.*\.gdbinit)' | sed -e 's:\./::' | sort > MANIFEST
