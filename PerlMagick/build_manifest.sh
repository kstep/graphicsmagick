#!/bin/sh
find . -type f -print | egrep -v '(.#.*)|(CVS)|(core)|(~$)|(.*\.cvsignore)|(.*\.gdbinit)|(demo/demo.jpg)|(Makefile.in)|(Makefile.old)' | sed -e 's:\./::' | sort > MANIFEST
