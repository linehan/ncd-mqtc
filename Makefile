CC=gcc
#
#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall            
LDFLAGS=-lm


SOURCES=main.c		 \
	string/strings.c \
	string/mtf.c	 \
	string/rotate.c  \
	tree/tt.c	 \
	tree/tt_print.c  \
	random/mersenne.c

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=r

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out 
