


/******************************************************************************
 * TREE CREATE 
 ******************************************************************************/

/**
 * tt_tree_create()
 * ----------------
 * Create an entire tree from an NxN data matrix
 *
 * @n    : Number of data points
 * @data : @nx@n data matrix.
 * Return: Pointer to a tree structure.
 */
struct tt_t *tt_tree_create(int n, float **data)
{
        struct tt_t *tree;
        int i;

        tree = calloc(1, sizeof(struct tt_t));

        tree->root = tt_create();
        tree->data = data;

        for (i=0; i<n; i++) {
                tt_insert(tree->root, i);
                if (!tt_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        tree->count        = n;
        tree->num_leaves   = tt_count_leaves(tree->root); 
        tree->num_internal = tt_count_internal(tree->root); 
        tree->max_cost     = tt_get_cost_max(tree->root, tree->data, n); 
        tree->min_cost     = tt_get_cost_min(tree->root, tree->data, n); 

        return tree;
}



/******************************************************************************
 * TREE FREE 
 ******************************************************************************/

/* 
 * Used to collect the nodes of the tree, which
 * will be free'd.
 *
 * This could be done much more efficiently, so
 * do that at some point.
 */
struct tt_node **Mem;
int Mem_index;
int Mem_max;

/**
 * __impl__tt_tree_free()
 * ----------------------
 * Implements tt_tree_free()
 */
void __impl__tt_tree_free(struct tt_node *n, int i)
{
        if (n != NULL && Mem_index < Mem_max) {
                Mem[Mem_index++] = n;
        }
}

/**
 * tt_tree_free()
 * --------------
 * Remove a tree from memory.
 *
 * @tree : Pointer to tree structure.
 * Return: Nothing.
 */
void tt_tree_free(struct tt_t *tree)
{
        int i;

        Mem_max = tree->count + (tree->count-2);

        Mem       = calloc(Mem_max, sizeof(struct tt_node *));
        Mem_index = 0;

        tt_traverse_preorder(tree->root, __impl__tt_tree_free);

        for (i=0; i<Mem_index; i++) {
                tt_node_destroy(Mem[i]);
        }

        free(Mem);
        free(tree);
}


/******************************************************************************
 * TREE COPY 
 ******************************************************************************/

struct tt_node *This_root;

/**
 * __impl__tt_tree_copy()
 * ----------------------
 * Implements tt_tree_copy().
 */
void __impl__tt_tree_copy(struct tt_node *a, int i)
{
        struct tt_node *copy;

        if (a != NULL && !tt_is_root(a)) {
                copy = tt_copy(a); 

                char *path = tt_get_path(a);

                tt_insert_on_path(This_root, copy, path);

                free(path);
        }

        return;
}


/**
 * tt_tree_copy()
 * --------------
 * Copy an entire tree structure.
 *
 * @tree : Pointer to tree being copied.
 * Return: Pointer to new tree, identical to @tree. 
 */
struct tt_t *tt_tree_copy(struct tt_t *tree)
{
        struct tt_t *copy;

        copy = calloc(1, sizeof(struct tt_t));

        copy->data         = tree->data;
        copy->count        = tree->count;
        copy->num_leaves   = tree->num_leaves; 
        copy->num_internal = tree->num_internal; 
        copy->max_cost     = tree->max_cost;
        copy->min_cost     = tree->min_cost;

        copy->root = tt_copy(tree->root);

        This_root = copy->root;

        tt_traverse_preorder(tree->root, __impl__tt_tree_copy);

        return copy;
}



/******************************************************************************
 * TREE COST 
 ******************************************************************************/

float **Cost_data; 
float   Tree_cost; 


/**
 * __impl__tt_tree_cost()
 * ----------------------
 * Implements tt_tree_cost(). 
 */
void __impl__tt_tree_cost(struct tt_node *n, int i)
{
        if (tt_is_internal(n)) {
                Tree_cost += tt_get_cost(n, Cost_data);
        }
        return;
}


/**
 * tt_tree_cost()
 * --------------
 * Compute the un-normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost C(T) of the tree at @tree.
 */
float tt_tree_cost(struct tt_t *tree)
{
        Cost_data = tree->data;
        Tree_cost = 0;

        tt_traverse_preorder(tree->root, __impl__tt_tree_cost);

        return Tree_cost;
}


/**
 * tt_tree_cost_scaled()
 * ---------------------
 * Compute the normalized tree cost.
 *
 * @tree : Pointer to a tree structure.
 * Return: Cost S(T) of the tree at @tree.
 */
float tt_tree_cost_scaled(struct tt_t *tree)
{
        float Ct = tt_tree_cost(tree);
        return (tree->max_cost - Ct) / (tree->max_cost - tree->min_cost);
}




/******************************************************************************
 * TREE MUTATE 
 ******************************************************************************/

/**
 * tt_tree_mutate()
 * ----------------
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
int tt_tree_mutate(struct tt_t *tree, struct alias_t *alias)
{
        struct tt_node *a;
        struct tt_node *b;
        int r;
        int m;
        int i;

        m = alias_sample(alias)+1;

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = tt_random_leaf(tree->root);
                        b = tt_random_leaf(tree->root);
                        tt_LEAF_INTERCHANGE(a, b);
                        break;
                case 1:
                        a = tt_random_node(tree->root);
                        b = tt_random_node(tree->root);
                        tt_SUBTREE_INTERCHANGE(a, b);
                        break;
                case 2:
                        a = tt_random_node(tree->root);
                        b = tt_random_node(tree->root);
                        tt_SUBTREE_TRANSFER(a, b);
                        break;
                }

                if (!tt_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (tree->num_leaves != tt_count_leaves(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        return m;
}


/**
 * tt_tree_mutate_mmc()
 * --------------------
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
int tt_tree_mutate_mmc(struct tt_t *tree, struct alias_t *alias)
{
        struct tt_node *a;
        struct tt_node *b;
        float cost;
        float best;
        /*float accept;*/
        int r;
        int m;
        int i;

        m = alias_sample(alias)+1;

        best = tt_tree_cost(tree);

        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = tt_random_leaf(tree->root);
                        b = tt_random_leaf(tree->root);
                        tt_LEAF_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        cost = tt_tree_cost(tree);

                        if (uniform_random() < min_float(2, 1.0, cost/best)) {
                        /*if (cost > best) {*/
                                best = cost;
                        } else {
                                /* Undo mutation */
                                tt_LEAF_INTERCHANGE(b, a);
                        }
                        break;
                case 1:
                        a = tt_random_node(tree->root);
                        b = tt_random_node(tree->root);
                        tt_SUBTREE_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        cost = tt_tree_cost(tree);
                        /*if (cost > best) {*/
                        if (uniform_random() < min_float(2, 1.0, cost/best)) {
                                best = cost;
                        } else {
                                /* Undo mutation */
                                tt_SUBTREE_INTERCHANGE(b, a);
                        }
                        break;
                case 2:
                        a = tt_random_node(tree->root);
                        b = tt_random_node(tree->root);
                        tt_SUBTREE_TRANSFER(a, b);

                        /* Check cost and undo if bad step */
                        cost = tt_tree_cost(tree);
                        /*if (cost > best) {*/
                        if (uniform_random() < min_float(2, 1.0, cost/best)) {
                                best = cost;
                        } else {
                                /* Undo mutation */
                                tt_SUBTREE_INTERCHANGE(b, a);
                        }
                        break;
                }

                if (!tt_is_ternary(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (tree->num_leaves != tt_count_leaves(tree->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        return m;
}
        




/**
 * tt_tree_mutate_mmc2()
 * --------------------
 * Perform various mutations on a tree structure, preserving shape invariants.
 *
 * @tree : Pointer to a tree structure.
 * Return: Number of mutations made
 */
struct tt_t *tt_tree_mutate_mmc2(struct tt_t *tree, struct alias_t *alias, int *num_mutations)
{
        struct tt_node *a;
        struct tt_node *b;
        float cost;
        float init;
        float running;
        /*float accept;*/
        int r;
        int m;
        int i;

        struct tt_t *test;

        m = alias_sample(alias)+1;

        if (num_mutations != NULL) {
                *num_mutations = m;
        }

        init = tt_tree_cost(tree);
        test = tt_tree_copy(tree);
        running = init;


        for (i=0; i<m; i++) {

                r = dice_roll(3);

                switch (r) {
                case 0:
                        a = tt_random_leaf(test->root);
                        b = tt_random_leaf(test->root);
                        tt_LEAF_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        /*cost = tt_tree_cost(test);*/

                        /*if (uniform_random() < min_float(2, 1.0, cost/running)) {*/
                        /*[>[>if (cost > best) {<]<]*/
                                /*[>best = cost;<]*/
                                /*running = cost;*/
                        /*} else {*/
                                /*[>[> Undo mutation <]<]*/
                                /*tt_LEAF_INTERCHANGE(b, a);*/
                        /*}*/
                        break;
                case 1:
                        a = tt_random_node(test->root);
                        b = tt_random_node(test->root);
                        tt_SUBTREE_INTERCHANGE(a, b);

                        /* Check cost and undo if bad step */
                        /*cost = tt_tree_cost(test);*/
                        /*[>[>if (cost > best) {<]<]*/
                        /*if (uniform_random() < min_float(2, 1.0, cost/running)) {*/
                                /*[>best = cost;<]*/
                                /*running = cost;*/
                        /*} else {*/
                                /*[>[> Undo mutation <]<]*/
                                /*tt_SUBTREE_INTERCHANGE(b, a);*/
                        /*}*/
                        break;
                case 2:
                        a = tt_random_node(test->root);
                        b = tt_random_node(test->root);
                        tt_SUBTREE_TRANSFER(a, b);

                        /* Check cost and undo if bad step */
                        /*cost = tt_tree_cost(test);*/
                        /*[>[>if (cost > best) {<]<]*/
                        /*if (uniform_random() < min_float(2, 1.0, cost/running)) {*/
                                /*[>best = cost;<]*/
                                /*running = cost;*/
                        /*} else {*/
                                /*[>[> Undo mutation <]<]*/
                                /*tt_SUBTREE_INTERCHANGE(b, a);*/
                        /*}*/
                        break;
                }

                if (!tt_is_ternary(test->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }

                if (test->num_leaves != tt_count_leaves(test->root)) {
                        fprintf(stderr, "Malformed tree.\n");
                        exit(1);
                }
        }

        cost = tt_tree_cost(test);

        if (uniform_random() < cost/init) {
                tt_tree_free(tree);
                return test;
        } else {
                tt_tree_free(test);
                return tree;
        }

        /*return m;*/
}
