
CC=gcc
CFLAGS=-g -pg -Wall -std=c99 -fms-extensions
LIBS=-lGL -lglut -lGLU

prog: main.o matrix.o list.o draw.o polygon.o
	${CC} ${CFLAGS} ${LIBS} polygon.o matrix.o main.o list.o draw.o -o prog
