CC=gcc
#
#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall            
LDFLAGS=-lm


SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=random

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out 
