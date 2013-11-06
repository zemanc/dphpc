# Makefile for n-particle problem
CC = g++
CFLAGS = -Wall -pedantic -ansi -c
LFLAGS = -Wall
nBody: main.o domain.o
	$(CC) $(LFLAGS) -o $@ $^
main.o: main.cpp domain.h ArgumentParser.h Timer.h
	$(CC) $(CFLAGS) $<
domain.o: domain.cpp domain.h
	$(CC) $(CFLAGS) $<
clean: 
	$(RM) nBody *.o *.out
