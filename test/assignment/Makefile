all: test test_vebtree

CC = g++
OPT= -g -flto -Ofast
CFLAGS = $(OPT) -Wall -march=native
LIBS = -lssl -lcrypto
DEBUG = -DDEBUG_OUTPUT

test: test.cc
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

test_vebtree: test_vebtree.cc
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(DEBUG)

clean:
	rm -f test test.o
	rm -f test_vebtree*
