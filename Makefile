NAME=iptrail
CC=gcc
CCFLAGS=-g -Wall -lGeoIP -DDEBUG

OBJS=iptrail.o geoip.o trace.o
SRC=src

all: ${OBJS}
	${CC} ${CCFLAGS} ${OBJS} -o ${NAME}

iptrail.o: ${SRC}/iptrail.[ch]
	${CC} -c ${CCFLAGS} ${SRC}/iptrail.c

geoip.o: ${SRC}/geoip.[ch]
	${CC} -c ${CCFLAGS} ${SRC}/geoip.c

trace.o: ${SRC}/trace.[ch]
	${CC} -c ${CCFLAGS} ${SRC}/trace.c

clean:
	rm -f ${OBJS} ${NAME}
