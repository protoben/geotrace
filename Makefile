NAME=geotrace
CC=gcc
CCFLAGS=-lGeoIP -lcap
CCDBG=-g -Wall -DDEBUG

OBJS=geotrace.o ipdata.o trace.o
SRC=src

all: ${OBJS}
	${CC} ${CCFLAGS} ${DBG} ${OBJS} -o ${NAME}

geotrace.o: ${SRC}/geotrace.[ch]
	${CC} -c ${CCFLAGS} ${DBG} ${SRC}/geotrace.c

ipdata.o: ${SRC}/ipdata.[ch]
	${CC} -c ${CCFLAGS} ${DBG} ${SRC}/ipdata.c

trace.o: ${SRC}/trace.[ch]
	${CC} -c ${CCFLAGS} ${DBG} ${SRC}/trace.c

clean:
	rm -f ${OBJS} ${NAME}
