#  RAL-CGM Interpreter - include Makefile
#
#    SYSTEM settings for known machines as follows:
#
#             (Topaz)      - topaz
#             (Sun 3 or 4) - sun
#             (Iris 3130)  - iris
#             (Iris 4D)    - iris4d
#             (Iris Indigo)- indigo
#             (IBM RS6000) - rs6000
#             (DECstation) - dec
#             (Unicos)     - cray
#             (VAX VMS)    - vms
#
SYSTEM =

ralcgm:	mach.h 

mach.h:	mc_$(SYSTEM).h Makefile
	cp mc_$(SYSTEM).h mach.h

clean:
	rm -f mach.h core a.out
