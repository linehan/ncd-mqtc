#include "ytree.h"

/******************************************************************************
 * NODE COST  
 ******************************************************************************/

/**
 * ynode_get_cost()
 * ---------------- 
 * Determine the cost value of a particular node.
 *
 * @n       : Pointer to node.
 * @distance: Distance matrix from which to compute the cost.
 * Return   : Cost value of @n.
 */
float ynode_get_cost(struct ynode_t *n, float **distance)
{
        struct ynode_t *root;

        ynode_value_t *value_L; 
        ynode_value_t *value_R;
        ynode_value_t *value_P;

        int count_L;
        int count_R;
        int count_P;

        int combi_L;
        int combi_R;
        int combi_P;

        float distance_LR = 0.0;
        float distance_PL = 0.0;
        float distance_PR = 0.0;

        float cost_LR;
        float cost_PL;
        float cost_PR;

        float cost = 0.0;

        int i;
        int j;

        if (n == NULL) {
                return -1.0;
        }

        root = ynode_get_root(n);

        count_L = ynode_count_leaves(n->L);
        count_R = ynode_count_leaves(n->R);
        count_P = ynode_count_leaves_not(root, n);

        combi_L = binomial(count_L, 2);
        combi_R = binomial(count_R, 2);
        combi_P = binomial(count_P, 2);

        value_L = ynode_get_values(n->L);
        value_R = ynode_get_values(n->R);
        value_P = ynode_get_values_not(root, n);

        for (i=0; i<count_L; i++) {
                for (j=0; j<count_R; j++) {
                        if (value_L[i] == value_R[j]) {
                                printf("CHAOS REIGNS\n");
                                abort();
                        }
                        if (value_L[i]<=DATA_COUNT && value_R[j]<=DATA_COUNT) {
                                distance_LR += distance[value_L[i]][value_R[j]];
                        }
                }
        }

        for (i=0; i<count_P; i++) {
                for (j=0; j<count_L; j++) {
                        if (value_P[i] == value_L[j]) {
                                printf("CHAOS REIGNS\n");
                                abort();
                        }
                        if (value_P[i]<=DATA_COUNT && value_L[j]<=DATA_COUNT) {
                                distance_PL += distance[value_P[i]][value_L[j]];
                        }
                }
        }

        for (i=0; i<count_P; i++) {
                for (j=0; j<count_R; j++) {
                        if (value_P[i] == value_R[j]) {
                                printf("CHAOS REIGNS\n");
                                abort();
                        }
                        if (value_P[i]<=DATA_COUNT && value_R[j]<=DATA_COUNT) {
                                distance_PR += distance[value_P[i]][value_R[j]];
                        }
                }
        }

        cost_LR = (float)combi_P * distance_LR;
        cost_PL = (float)combi_R * distance_PL;
        cost_PR = (float)combi_L * distance_PR;

        cost = cost_LR + cost_PL + cost_PR;

        /*printf("COUNT l:%d r:%d p:%d\n", count_L, count_R, count_P);*/
        /*printf("BINOM l:%d r:%d p:%d\n", combi_L, combi_R, combi_P);*/
        /*printf("DIST  l:%f r:%f p:%f\n", distance_PR, distance_PL, distance_LR);*/
        /*printf("COST  l:%f r:%f p:%f\n", cost_PR, cost_PL, cost_LR);*/
        /*printf("TOTAL %f\n", cost);*/

        if (value_L != NULL) {
                free(value_L);
        }
        if (value_R != NULL) {
                free(value_R);
        }
        if (value_P != NULL) {
                free(value_P);
        }

        return cost;
}


/**
 * ynode_get_cost_max()
 * -------------------- 
 * Determine the maximum cost value of a particular node.
 *
 * @a    : Pointer to node.
 * @d    : Distance matrix from which to compute the cost.
 * @n    : Number of items, i.e. @d is an @nx@n matrix.
 * Return: Maximum cost value of @n.
 */
float ynode_get_cost_max(struct ynode_t *a, float **d, int n)
{
        int i;
        int j;
        int k;
        int l;

        int count;
        int combo;
        float cost;

        float ijkl; /* ij|kl quartet */
        float ikjl; /* ik|jl quartet */
        float iljk; /* il|jk quartet */

        cost  = 0.0;
        count = 0;
        combo = binomial(n, 4);

        for (i=0; i<n; i++) {
                for (j=(i+1); j<n; j++) {
                        for (k=(j+1); k<n; k++) {
                                for (l=(k+1); l<n; l++) {

                                        ijkl = d[i][j] + d[k][l];
                                        ikjl = d[i][k] + d[j][l];
                                        iljk = d[i][l] + d[j][k];

                                        /*printf("max(%f, %f, %f) := %f\n", ijkl, ikjl, iljk, max_float(3, ijkl, ikjl, iljk));*/

                                        cost += max_float(3, ijkl, ikjl, iljk);

                                        count++;
                                }
                        }
                }
        }

        if (count != combo) {
                fprintf(stderr, "Combination error.\n");
                return -1.0;
        }

        /*printf("max cost:%f\n", cost);*/

        return cost;
}


/**
 * ynode_get_cost_min()
 * -------------------- 
 * Determine the minimum cost value of a particular node.
 *
 * @a    : Pointer to node.
 * @d    : Distance matrix from which to compute the cost.
 * @n    : Number of items, i.e. @d is an @nx@n matrix.
 * Return: Minimum cost value of @n.
 */
float ynode_get_cost_min(struct ynode_t *a, float **d, int n)
{
        int i;
        int j;
        int k;
        int l;

        int count;
        int combo;
        float cost;

        float ijkl; /* ij|kl quartet */
        float ikjl; /* ik|jl quartet */
        float iljk; /* il|jk quartet */

        cost  = 0.0;
        count = 0;
        combo = binomial(n, 4);

        for (i=0; i<n; i++) {
                for (j=(i+1); j<n; j++) {
                        for (k=(j+1); k<n; k++) {
                                for (l=(k+1); l<n; l++) {

                                        ijkl = d[i][j] + d[k][l];
                                        ikjl = d[i][k] + d[j][l];
                                        iljk = d[i][l] + d[j][k];

                                        /*printf("min(%f, %f, %f) := %f\n", ijkl, ikjl, iljk, min_float(3, ijkl, ikjl, iljk));*/

                                        cost += min_float(3, ijkl, ikjl, iljk);

                                        count++;
                                }
                        }
                }
        }

        if (count != combo) {
                fprintf(stderr, "Combination error.\n");
                return -1.0;
        }

        /*printf("min cost:%f\n", cost);*/

        return cost;
}


/**
 * ynode_get_cost_scaled()
 * ----------------------- 
 * Scale the measured cost using the max/min cost. 
 *
 * @c    : Measured (unscaled) cost
 * @M    : Maximum possible cost
 * @m    : Minimum possible cost
 * Return: Normalized value on interval (0,1).
 */
float ynode_get_cost_scaled(float c, float M, float m)
{
        return (M - c) / (M - m);
}

