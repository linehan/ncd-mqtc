#########################
# Configure build      
#########################

COMPILER=gcc
#
#  optimize   enable all  include
#  level 3    warnings	  paths     see note 
#         \    |           |         /
CC_FLAGS=-O3 -Wall $(INCLUDE) #-ffast-math 
LD_FLAGS=-lm -lz -lbz2
#	  /    |    \
#      math   zlib   bzlib
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

MQTC_SOURCES=src/cluster/main.c		\
	src/cluster/input.c		\
	src/cluster/logs.c		\
	src/cluster/util/list.c		\
	src/cluster/util/math.c 	\
	src/cluster/prng/mersenne.c	\
	src/cluster/prng/prng.c		\
	src/cluster/prng/coin.c		\
	src/cluster/prng/dice.c		\
	src/cluster/prng/alias.c	\
	src/cluster/tree/node_alloc.c	\
	src/cluster/tree/node_check.c	\
	src/cluster/tree/node_cost.c	\
	src/cluster/tree/node_traverse.c\
	src/cluster/tree/node_count.c	\
	src/cluster/tree/node_get.c	\
	src/cluster/tree/node_insert.c	\
	src/cluster/tree/node_mutate.c	\
	src/cluster/tree/node_print.c	\
	src/cluster/tree/tree_alloc.c	\
	src/cluster/tree/tree_cost.c	\
	src/cluster/tree/tree_mutate.c	\

MQTC_OBJECTS=$(MQTC_SOURCES:.c=.o)


NCD_SOURCES=src/compress/main.c 	\
	src/compress/filesystem.c 	\
	src/compress/ncd.c 		\
	src/compress/module/mod_zlib.c 	\
	src/compress/module/mod_bzlib.c

NCD_OBJECTS=$(NCD_SOURCES:.c=.o)


#########################
# Configure rules 
#########################
all: cluster compress 

cluster: $(MQTC_SOURCES)
	$(COMPILER) $(CC_FLAGS) $(MQTC_SOURCES) -o cluster $(LD_FLAGS)

compress: $(NCD_SOURCES)
	$(COMPILER) $(CC_FLAGS) $(NCD_SOURCES) -o compress $(LD_FLAGS)

clean:
	rm -f $(MQTC_OBJECTS) $(NCD_OBJECTS) cluster compress gmon.out 
