# This file is only here for the recompilation of systems where
# the source, but not the configuration, is being changed.  It
# is a maintenance tool.
#
# CGMinstall and CGMconfig should be used before this is ever run.
#
SRC = ./src

ralcgm cgm:
	cd $(SRC); $(MAKE) ralcgm

cgmlib:
	cd $(SRC); $(MAKE) cgmlib

all:
	$(MAKE) ralcgm
	$(MAKE) cgmlib

clean:
	cd $(SRC); $(MAKE) clean
