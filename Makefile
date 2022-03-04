CC = gcc 
TARGET = lsc.c -o lsc
ERRORS = -Wall -Wextra -pedantic-errors

.PHONY: clean

lsc: lsc.c
		${CC} ${TARGET} -O3

debug: lsc.c
		${CC} ${TARGET} ${ERRORS} -Og

clean:
		rm -f lsc	
