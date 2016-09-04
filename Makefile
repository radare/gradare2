#include ../config.mk

BIN=gradare2${BINSFX}
OBJ=main.o topbar.o toolbar.o actions.o execute.o prefs.o menubar.o dialog.o
CFLAGS+=-DPREFIX=\"${PREFIX}\" -DGRSCDIR=\"./grsc/\"
#\"${DATADIR}/radare/gradare\"
CFLAGS+=${GTK_FLAGS} ${VTE_FLAGS}
CFLAGS+=-DVERSION=\"0.1\"
USE_GTK2=0
CFLAGS+=-DUSE_GTK2=$(USE_GTK2)
ifeq ($(USE_GTK2),1)
GTK_FLAGS=`pkg-config glib-2.0 gtk+-2.0 vte-2.91 --cflags`
GTK_LIBS=`pkg-config glib-2.0 gtk+-2.0 vte-2.91 --libs`
else
GTK_FLAGS=`pkg-config glib-2.0 gtk+-3.0 vte-2.91 --cflags`
GTK_LIBS=`pkg-config glib-2.0 gtk+-3.0 vte-2.91 --libs`
endif
LIBS+=${GTK_LIBS} ${VTE_LIBS}
GRDIR=${DESTDIR}/${DATADIR}/gradare2/


all: ${BIN}
	@echo 'Linking ${BIN}'

${BIN}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJ} ${LIBS} -o ${BIN}

${OBJ}: %.o: %.c
	@echo 'Compiling $<'
	${CC} ${CFLAGS} -c -o $@ $<


install:
	@echo 'Installing gradare2'
	${INSTALL_DIR} ${DESTDIR}/${PREFIX}/bin
	-${INSTALL_PROGRAM} ${BIN} ${DESTDIR}/${PREFIX}/bin
	-rm -rf ${GRDIR}/*
	mkdir -p ${GRDIR}
	cp -rf grsc/* ${GRDIR}/
	-chmod +x ${GRDIR}/Shell

clean:
	@echo 'Cleaning gui'
	-rm -f ${OBJ} ${BIN}
