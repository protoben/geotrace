NAME=iptrail
CC=gcc
CCFLAGS=-g -Wall

OBJS=iptrail.o
SRC=src

all: iptrail.o
	${CC} ${CCFLAGS} ${OBJS} -o ${NAME}

iptrail.o: ${SRC}/iptrail.c ${SRC}/iptrail.h
	${CC} -c ${CCFLAGS} ${SRC}/iptrail.c

clean:
	rm -f ${OBJS} ${NAME}
