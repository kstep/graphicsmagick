#
# Generate message dictionaries from XML master file
#

all: locale_c.h gm_messages.mc

# Add '-debug locale' to convert commands to see verbose listing
locale_c.h : locale.mgk C.mgk
	gm convert locale:locale.mgk localeh:locale_c.h

gm_messages.mc: locale.mgk C.mgk
	gm convert locale:locale.mgk localemc:gm_messages.mc

install: locale_c.h gm_messages.mc
	cp locale_c.h ../magick
	cp gm_messages.mc ../magick

clean:
	rm -f locale_c.h gm_messages.mc