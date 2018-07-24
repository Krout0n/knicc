CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

compiler: $(OBJS)
		gcc -o compiler $^

$(OBJS): knicc.h

test: compiler
		./test.sh

clean:
		rm -f compiler *.o *~ *.s
