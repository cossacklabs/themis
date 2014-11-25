.PHONY: boolt testalt booltest boolbad hoof testalt uconstants ull baz

SPLINT = splint

# ull and baz added after 2.5q
all: testalt boolt uconstants badcomment hoof boolbad booltest ull baz immutable impabsmodule

testalt:
	-${SPLINT} -dump newlint.lcd testalt.c

boolt:
	-${SPLINT} -expect 1 +partial +booltype pan_bool_t +booltrue PAN_TRUE +boolfalse PAN_FALSE boolt.c

uconstants:
	-${SPLINT} uconstants.c -expect 2

badcomment:
	-${SPLINT} badcomment.c -expect 3

hoof:
	-${SPLINT} hoof.c -expect 1
	-${SPLINT} hoof.c +ignorequals

ignorecmd:
	-${SPLINT} +ignorecommand -we

booltest:
	-${SPLINT} booltest.c -booltype bool -expect 2
	-${SPLINT} booltest.c -booltype bool +boolint
	-${SPLINT} booltest.c -booltype bool -predboolint -expect 1

boolbad:
	-${SPLINT} -weak -booltype BOOLEAN  -booltrue TRUE -boolfalse FALSE boolbad.c


ull:
	-${SPLINT} ull.c

baz:
	-${SPLINT} baz.h bimbim.h -I.

immutable:
	-${SPLINT} immutable.c testimmutable.c -expect 2

impabsmodule:
	-${SPLINT} +impabstract impabsmodule.c -expect 2
