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

MQTC_SOURCES=src/mqtc/main.c		\
	src/mqtc/input.c		\
	src/mqtc/logs.c			\
	src/mqtc/util/list.c		\
	src/mqtc/util/math.c 		\
	src/mqtc/prng/mersenne.c	\
	src/mqtc/prng/prng.c		\
	src/mqtc/prng/coin.c		\
	src/mqtc/prng/dice.c		\
	src/mqtc/prng/alias.c		\
	src/mqtc/tree/node_alloc.c	\
	src/mqtc/tree/node_check.c	\
	src/mqtc/tree/node_cost.c	\
	src/mqtc/tree/node_traverse.c	\
	src/mqtc/tree/node_count.c	\
	src/mqtc/tree/node_get.c	\
	src/mqtc/tree/node_insert.c	\
	src/mqtc/tree/node_mutate.c	\
	src/mqtc/tree/node_print.c	\
	src/mqtc/tree/tree_alloc.c	\
	src/mqtc/tree/tree_cost.c	\
	src/mqtc/tree/tree_mutate.c	\

MQTC_OBJECTS=$(MQTC_SOURCES:.c=.o)


NCD_SOURCES=src/ncd/main.c 		\
	src/ncd/filesystem.c 		\
	src/ncd/ncd.c 			\
	src/ncd/module/mod_zlib.c 	\
	src/ncd/module/mod_bzlib.c

NCD_OBJECTS=$(NCD_SOURCES:.c=.o)


#########################
# Configure rules
#########################
all: mqtc compress

mqtc: $(MQTC_SOURCES)
	$(COMPILER) $(CC_FLAGS) $(MQTC_SOURCES) -o mqtc $(LD_FLAGS)

ncd: $(NCD_SOURCES)
	$(COMPILER) $(CC_FLAGS) $(NCD_SOURCES) -o ncd $(LD_FLAGS)

clean:
	rm -f $(MQTC_OBJECTS) $(NCD_OBJECTS) mqtc ncd gmon.out
