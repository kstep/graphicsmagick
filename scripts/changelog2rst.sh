#!/bin/sh
# Simple filter to filter a ChangeLog to something in reStructuredText.
#
sed -e 's/	\* /  - /g ; s/	/    / ; s/\*/\\*/g; s/\_/\\_/g'
