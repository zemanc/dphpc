CC=clang
CFLAGS=-O2 -std=gnu99
LDLIBS=-lpthread -lrt

BINARIES=stack 

all: $(BINARIES)

stack: stack.c

run: stack
	./stack | tee results.dat
	cat stack-lock-analysis.R | R --no-save
	evince stack-lock-analysis.pdf

clean:
	rm -f $(BINARIES) *.o
