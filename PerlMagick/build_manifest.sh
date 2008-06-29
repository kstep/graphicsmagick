#!/bin/sh
find . -type f -print | egrep -v '(.#.*)|(CVS)|(core)|(~$)|(.*\.cvsignore)|(.*\.gdbinit)|(demo/demo.jpg)' | sed -e 's:\./::' | sort > MANIFEST
