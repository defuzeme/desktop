CC   = rcc

QRCS =	starter.qrc \
		about.qrc \
		lastfm.qrc

RCCS = $(QRCS:.qrc=.rcc)
.SUFFIXES: .qrc .rcc

all: $(RCCS)

move: all
	mv $(RCCS) "../../client-build-desktop/resources/."

move_win: all
	move *.rcc "../../client-build-desktop/resources/."

move_mac: all
	mv $(RCCS) "../../client-build-desktop/client_`uname -m`.app/Contents/resources/."

##########################

.qrc.rcc:
	$(CC) -binary $< -o $@
