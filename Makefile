NAME=geotrace
CC=gcc
CFLAGS=-lGeoIP -lcap
DBG=-g -Wall -DDEBUG

OBJS=geotrace.o ipdata.o trace.o
SRC=src

all: ${OBJS}
	${CC} ${CFLAGS} ${DBG} ${OBJS} -o ${NAME}

%.o: ${SRC}/%.c ${SRC}/%.h
	${CC} -c ${CFLAGS} ${DBG} $< -o $@

clean:
	rm -f ${OBJS} ${NAME}
