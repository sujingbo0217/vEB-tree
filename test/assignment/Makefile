all: test

CC = g++
OPT = -g -flto -Ofast -std=c++20
CFLAGS = $(OPT) -Wall -march=native
LIBS = -lssl -lcrypto
DEBUG = -DDEBUG_OUTPUT

test: test.cc	../../source/vebtree/vebtree.cpp
	$(CC) $(CFLAGS) test.cc ../../source/vebtree/vebtree.cpp -o test $(LIBS)

clean:
	rm -f test test.o
