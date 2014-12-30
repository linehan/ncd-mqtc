#########################
# Configure project     
#########################
PROGRAM=r
INCLUDE=


#########################
# Configure build      
#########################

COMPILER=gcc
#
#  optimize   enable all
#  level 3    warnings    include paths
#         \    |           /
CC_FLAGS=-O3 -Wall $(INCLUDE)
LD_FLAGS=-lm
#	  /
#      math	 
#

#########################
# Configure files 
#########################

SOURCES=main.c			\
	string/strings.c	\
	string/mtf.c		\
	string/rotate.c		\
	tree/tt.c		\
	tree/tt_print.c		\
	random/mersenne.c	\
	random/math.c 

STATICS=
OBJECTS=$(SOURCES:.c=.o)


#########################
# Configure rules 
#########################
all: $(SOURCES)
	$(COMPILER) $(CC_FLAGS) $(SOURCES) $(STATICS) -o $(PROGRAM) $(LD_FLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAM) gmon.out 
