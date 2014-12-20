CC=gcc
#
#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall            
LDFLAGS=-lm


SOURCES=main.c \
	strings.c \
	mtf.c \
	rotate.c \
	tt.c \
	tt_print.c \
	stoc/dice.c \
	stoc/mersenne.c \
	stoc/shuffle.c \
	stoc/stoc.c

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=random

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out 
