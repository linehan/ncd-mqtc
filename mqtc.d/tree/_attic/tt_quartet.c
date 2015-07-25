#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "../random/mersenne.h"
#include "tt.h"
#include "tt_algo.h"

/*
 * Hierarchical Clustering Algorithm
 * ================================= 
 *
 * REFERENCE
 * ---------
 *      A Fast Quartet tree heuristic for hierarchical clustering
 *      R.L. Cilibrasi, P.M.B. Vitanyi 
 *      Pattern Recognition 44 (2011) 662-677
 *
 * DEFINITIONS
 * -----------
 * 
 * Let N be a set of n elements.
 *                                                      u       w 
 * Let F be a family of 4-element subsets of N           \     /
 *      |F| = n-choose-4.                                 1---2
 *                                                       /     \
 * For each {u,v,w,x} in F there are 3 equal            v       x
 * partitions: uv|wx, uw|vx, ux|vw.           
 *                                                       
 * For each partition we may construct an               u       w
 * unrooted binary tree, which we will call              \     /
 * a quartet topology.                                    1---2
 *                                                       /     \
 * Let Q be the set of 3*(n-choose-4) quartet           x       v
 * topologies induced by N.           
 *                                                      
 * Consider the class UBT of undirected trees           u       v
 * of arity 3 with n>=4 leaves, labeled with             \     /
 * the elements of N.                                     1---2
 *                                                       /     \
 * Such trees have                                      w       x
 *      n   leaf nodes     (degree 1)
 *      n-2 internal nodes (degree 3)
 * 
 * For tree T in UBT and u,v,w,x in N, we say
 *      
 *      "T is consistent with uv|wx", 
 *
 *      or 
 *      
 *      "uv|wx is embedded in T", 
 * 
 *      iff
 *
 *      the path from u to v does not cross
 *      the path from w to x. (i.e. u and v
 *      are siblings, w and x are siblings.)
 *
 * Precisely one of the three possible quartet
 * topologies will be consistent for a given
 * tree T from UBT.
 *
 * Therefore a tree from UBT contains precisely
 * n-choose-4 different quartet topologies.
 *
 *
 * ALGORITHM 
 * ---------
 *
 * 0. 
 * Create a random tree T in UBT from a data set N.
 *      i.   n   leaf nodes
 *      ii.  n-2 internal nodes
 *      iii. Each internal node has degree exactly 3 
 *
 *      The number of T in UBT is (2n-5)!!.
 *
 * 1. 
 * For tree T, 
 *      i.   Compute the summed total cost of all
 *           embedded quartet topologies.
 *      ii.  Compute S(T).
 *
 * 2.
 * Set the best known tree variable T to T_0. 
 *
 * 3.
 * Draw a number k with probability 
 *      p(1) = 1-c 
 *      p(k) = c/(k(log(k))^2)
 *     
 *      For k >= 2, where 1/c= SUM[k=2, inf.](1/(k(log(k)^2))).
 *
 *      (It is known that 1/c ~= 2.1.)
 *
 * 4. 
 * Compose a k-mutation.
 *
 * 5.
 *      i.  Apply the k-mutation to T_0 to obtain T
 *      ii.  Compute S(T). 
 *      iii. If S(T)>S(T_0), Set T_0 to T. 
 *
 * 6.
 * If S(T_0) = 1 or a termination condition holds, output
 * T_0 and halt. Otherwise go to step 3.
 */










