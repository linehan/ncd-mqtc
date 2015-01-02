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
#  optimize   enable all  include
#  level 3    warnings	  paths     see note 
#         \    |           |         /
CC_FLAGS=-O3 -Wall $(INCLUDE) #-ffast-math 
LD_FLAGS=-lm		
#	  /		
#      math	 
#
#
# NOTE on -ffast-math
#
# First, breaks strict IEEE compliance, e.g. allows re-ordering of 
# instructions to a mathematical equivalent, which may not be IEEE
# floating-point equivalent. 
#
# Second, disables setting errno after single-instruction math functions, 
# avoiding a write to a thread-local variable (can produce 100% speedup on
# certain architectures). 
#
# Third, assumes finite math only, meaning no checks for NaN (or 0) are 
# made where they would normally be. It is assumed these values will never 
# appear. 
#
# Fourth, enables reciprocal approximations for division and reciprocal 
# square root.
#
# Fifth, disables signed zero (even if the compile target supports it) 
# and rounding math, which enables optimizations e.g. constant folding.
#
# Sixth, generates code assuming no hardware interrupts occur in math
# due to signal()/trap(). If these cannot be disabled on the compile
# target and consequently occur, they will not be handled.
#

#########################
# Configure files 
#########################

SOURCES=main.c			\
	util/bnfop.c		\
	string/strings.c	\
	string/mtf.c		\
	string/rotate.c		\
	tree/tt.c		\
	tree/tt_print.c		\
	list/list.c		\
	math/mersenne.c		\
	math/coin.c		\
	math/dice.c		\
	math/alias.c		\
	math/math.c 

STATICS=
OBJECTS=$(SOURCES:.c=.o)


#########################
# Configure rules 
#########################
all: $(SOURCES)
	$(COMPILER) $(CC_FLAGS) $(SOURCES) $(STATICS) -o $(PROGRAM) $(LD_FLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAM) gmon.out 
