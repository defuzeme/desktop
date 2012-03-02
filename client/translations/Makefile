CC   = lrelease

TSS =	defuzeme_fr.ts

QMS = $(TSS:.ts=.qm)
.SUFFIXES: .ts .qm

all: $(QMS)

move: all
	mv $(QMS) "../../client-build-desktop/translations/."

move_win: all
	move *.qm "../../client-build-desktop/translations/."

move_mac: all
	mv $(QMS) "../../client-build-desktop/client_`uname -m`.app/Contents/translations/."

##########################

.ts.qm:
	$(CC) $<
