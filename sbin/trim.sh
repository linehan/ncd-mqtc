#!/bin/sh

#                          
#   Replace tab characters
#   with single spaces 
#           |   
cat $1 | unexpand | sed -e 's/[ \t]*$//'
#                          |
#                       Remove trailing whitespace characters from lines
#                       (causes bug in input.c of src/cluster), see the
#                       warning in in read_square_matrix())


