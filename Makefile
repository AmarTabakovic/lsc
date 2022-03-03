CC = gcc 
TARGET = lsc.c -o lsc
ERRORS = -Wall -Wextra -pedantic-errors

lsc: lsc.c
		${CC} ${TARGET} 

debug: lsc.c
		${CC} ${TARGET} ${ERRORS}
