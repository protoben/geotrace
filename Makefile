NAME=iptrail
CC=gcc
CCFLAGS=-g -Wall -lGeoIP

OBJS=iptrail.o geoip.o
SRC=src

all: ${OBJS}
	${CC} ${CCFLAGS} ${OBJS} -o ${NAME}

iptrail.o: ${SRC}/iptrail.[ch]
	${CC} -c ${CCFLAGS} ${SRC}/iptrail.c

geoip.o: ${SRC}/geoip.[ch]
	${CC} -c ${CCFLAGS} ${SRC}/geoip.c

clean:
	rm -f ${OBJS} ${NAME}
