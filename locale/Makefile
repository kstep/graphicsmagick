# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Generate message dictionaries from XML master file
#

# Uncomment for verbose listing
# DEBUG=-debug locale

LMC_TARGET=gm_messages.mc
LMC_INSTALL_TARGET=../magick/$(LMC_TARGET)
LCH_TARGET=locale_c.h
LCH_INSTALL_TARGET=../magick/$(LCH_TARGET)

LOCALE_XML_MASTER=locale.mgk
LOCALE_XML_DEPENDS=locale.mgk C.mgk

all: $(LCH_TARGET) $(LMC_TARGET)

# Add '-debug locale' to convert commands to see verbose listing
$(LCH_TARGET) : $(LOCALE_XML_DEPENDS)
	gm convert $(DEBUG) locale:$(LOCALE_XML_MASTER) localeh:$(LCH_TARGET)

$(LMC_TARGET): $(LOCALE_XML_DEPENDS)
	gm convert $(DEBUG) locale:$(LOCALE_XML_MASTER) localemc:$(LMC_TARGET)

$(LMC_INSTALL_TARGET) : $(LMC_TARGET)
	cp $(LMC_TARGET) $(LMC_INSTALL_TARGET)

$(LCH_INSTALL_TARGET) : $(LCH_TARGET)
	cp $(LCH_TARGET) $(LCH_INSTALL_TARGET)

install: $(LMC_INSTALL_TARGET) $(LCH_INSTALL_TARGET)

clean:
	rm -f $(LCH_TARGET) $(LMC_TARGET)