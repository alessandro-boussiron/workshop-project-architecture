/*
 * EXERCISE 8: LIMIT POINTER INDIRECTION
 * 
 * Task: Simplify pointer usage
 * - Maximum 2 levels of indirection
 * - Use structs instead of deep pointers
 * - Clear ownership and lifetime
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex08_pointer_indirection.c -o ex08
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_NODES 10
#define MAX_STRING_LENGTH 64

// ============================================
// ❌ BAD CODE TO FIX - TOO MUCH INDIRECTION
// ============================================

/* Problem 1: Triple pointer (3 levels) */
void bad_allocate_3d_array(int ****array, size_t x, size_t y, size_t z) {
    *array = malloc(x * sizeof(int**));
    for (size_t i = 0; i < x; i++) {
        (*array)[i] = malloc(y * sizeof(int*));
        for (size_t j = 0; j < y; j++) {
            (*array)[i][j] = malloc(z * sizeof(int));
        }
    }
}

void bad_free_3d_array(int ***array, size_t x, size_t y) {
    for (size_t i = 0; i < x; i++) {
        for (size_t j = 0; j < y; j++) {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

/* Problem 2: Pointer to pointer to pointer for tree */
typedef struct TreeNode {
    int value;
    struct TreeNode **children;  // Pointer to array of pointers
    size_t child_count;
} TreeNode;

void bad_add_child(TreeNode ***parent, TreeNode **child) {
    // Too complex!
    (**parent)->child_count++;
    (**parent)->children = realloc((**parent)->children, 
                                   (**parent)->child_count * sizeof(TreeNode*));
    (**parent)->children[(**parent)->child_count - 1] = *child;
}

/* Problem 3: Confusing ownership with multiple indirection */
typedef struct {
    char ***data;  // 3 levels!
    size_t ***indices;  // 3 levels!
} BadDataStructure;

// ============================================
// ✅ YOUR TASK: SIMPLIFY POINTER USAGE
// ============================================

/* TODO: Fix problem 1 - Replace 3D array with flat structure
 * Requirements:
 * - Use single malloc for contiguous memory
 * - Maximum 2 levels of indirection
 * - Simple indexing function
 * - Static allocation if possible
 */
typedef struct {
    int *data;  // Flat array
    size_t width;
    size_t height;
    size_t depth;
} Array3D;

void good_array3d_init(Array3D *array, size_t x, size_t y, size_t z) {
    // TODO: Initialize with flat allocation
    (void)array;
    (void)x;
    (void)y;
    (void)z;
}

void good_array3d_set(Array3D *array, size_t x, size_t y, size_t z, int value) {
    // TODO: Calculate flat index and set
    // index = x * (height * depth) + y * depth + z
    (void)array;
    (void)x;
    (void)y;
    (void)z;
    (void)value;
}

int good_array3d_get(const Array3D *array, size_t x, size_t y, size_t z) {
    // TODO: Calculate flat index and get
    (void)array;
    (void)x;
    (void)y;
    (void)z;
    return 0;
}

void good_array3d_cleanup(Array3D *array) {
    // TODO: Simple cleanup
    (void)array;
}

/* TODO: Fix problem 2 - Simplify tree structure
 * Requirements:
 * - Fixed maximum children per node
 * - Or use indices instead of pointers
 * - Maximum 2 levels of indirection
 * - Clear ownership
 */

// Option 1: Fixed children array
typedef struct SimpleTreeNode {
    int value;
    struct SimpleTreeNode *children[MAX_NODES];  // Fixed array
    size_t child_count;
} SimpleTreeNode;

void good_tree_add_child(SimpleTreeNode *parent, SimpleTreeNode *child) {
    // TODO: Add child with simple pointers
    (void)parent;
    (void)child;
}

// Option 2: Index-based tree (even better!)
typedef struct {
    int value;
    int parent_index;  // -1 for root
    int first_child_index;  // -1 for none
    int next_sibling_index;  // -1 for none
    bool active;
} IndexedTreeNode;

typedef struct {
    IndexedTreeNode nodes[MAX_NODES];
    size_t count;
} IndexedTree;

void good_indexed_tree_init(IndexedTree *tree) {
    // TODO: Initialize tree pool
    (void)tree;
}

int good_indexed_tree_add_node(IndexedTree *tree, int value, int parent_idx) {
    // TODO: Add node and return its index
    (void)tree;
    (void)value;
    (void)parent_idx;
    return -1;
}

/* TODO: Fix problem 3 - Replace with clear structure
 * Requirements:
 * - Use struct with clear members
 * - Maximum 2 levels of indirection
 * - Clear data ownership
 * - Self-documenting
 */
typedef struct {
    char strings[MAX_NODES][MAX_STRING_LENGTH];  // No pointers!
    size_t string_count;
    int indices[MAX_NODES];  // Simple array
    size_t index_count;
} GoodDataStructure;

void good_data_add_string(GoodDataStructure *data, const char *str) {
    // TODO: Add string to structure
    (void)data;
    (void)str;
}

void good_data_add_index(GoodDataStructure *data, int index) {
    // TODO: Add index to structure
    (void)data;
    (void)index;
}

// ============================================
// ADVANCED: Linked List Simplification
// ============================================

/* Bad: Double pointer for list manipulation */
typedef struct ListNode {
    int value;
    struct ListNode *next;
} ListNode;

void bad_insert_after(ListNode ***node, int value) {
    // Too confusing!
    ListNode *new_node = malloc(sizeof(ListNode));
    new_node->value = value;
    new_node->next = (**node)->next;
    (**node)->next = new_node;
}

/* TODO: Good - Use single pointer and return new pointer */
typedef struct GoodListNode {
    int value;
    struct GoodListNode *next;
} GoodListNode;

GoodListNode* good_insert_after(GoodListNode *node, int value) {
    // TODO: Simpler interface with return value
    (void)node;
    (void)value;
    return NULL;
}

/* TODO: Even better - Use handles/indices */
typedef struct {
    int value;
    int next_index;  // -1 for end
    bool active;
} ListElement;

typedef struct {
    ListElement elements[MAX_NODES];
    int head_index;
    size_t count;
} IndexedList;

void good_list_init(IndexedList *list) {
    // TODO: Initialize list
    (void)list;
}

int good_list_append(IndexedList *list, int value) {
    // TODO: Append and return index
    (void)list;
    (void)value;
    return -1;
}

void good_list_remove(IndexedList *list, int index) {
    // TODO: Remove by index
    (void)list;
    (void)index;
}

// ============================================
// TEST HARNESS
// ============================================

void test_3d_array(void) {
    printf("Test 1: 3D Array\n");
    
    Array3D array = {0};
    good_array3d_init(&array, 3, 4, 5);
    
    printf("  Setting values...\n");
    good_array3d_set(&array, 0, 0, 0, 100);
    good_array3d_set(&array, 2, 3, 4, 200);
    
    printf("  Getting values: %d, %d\n",
           good_array3d_get(&array, 0, 0, 0),
           good_array3d_get(&array, 2, 3, 4));
    
    good_array3d_cleanup(&array);
    printf("  3D array operations successful\n\n");
}

void test_tree(void) {
    printf("Test 2: Tree Structure\n");
    
    // Test simple tree
    SimpleTreeNode root = {.value = 1, .child_count = 0};
    SimpleTreeNode child1 = {.value = 2, .child_count = 0};
    SimpleTreeNode child2 = {.value = 3, .child_count = 0};
    
    good_tree_add_child(&root, &child1);
    good_tree_add_child(&root, &child2);
    
    printf("  Simple tree: root has %zu children\n", root.child_count);
    
    // Test indexed tree
    IndexedTree indexed_tree = {0};
    good_indexed_tree_init(&indexed_tree);
    
    int root_idx = good_indexed_tree_add_node(&indexed_tree, 1, -1);
    good_indexed_tree_add_node(&indexed_tree, 2, root_idx);
    good_indexed_tree_add_node(&indexed_tree, 3, root_idx);
    
    printf("  Indexed tree: %zu nodes\n", indexed_tree.count);
    printf("\n");
}

void test_data_structure(void) {
    printf("Test 3: Data Structure\n");
    
    GoodDataStructure data = {0};
    
    good_data_add_string(&data, "Hello");
    good_data_add_string(&data, "World");
    good_data_add_index(&data, 42);
    good_data_add_index(&data, 99);
    
    printf("  Strings: %zu, Indices: %zu\n", 
           data.string_count, data.index_count);
    printf("\n");
}

void test_list(void) {
    printf("Test 4: Indexed List\n");
    
    IndexedList list = {0};
    good_list_init(&list);
    
    printf("  Appending values...\n");
    good_list_append(&list, 10);
    good_list_append(&list, 20);
    good_list_append(&list, 30);
    
    printf("  List size: %zu\n", list.count);
    printf("\n");
}

int main(void) {
    printf("EXERCISE 8: LIMIT POINTER INDIRECTION\n");
    printf("======================================\n\n");
    
    test_3d_array();
    test_tree();
    test_data_structure();
    test_list();
    
    printf("✅ Exercise 8 complete!\n");
    printf("\nHints:\n");
    printf("1. Use flat arrays instead of pointer-to-pointer\n");
    printf("2. Calculate indices: idx = x * (H * D) + y * D + z\n");
    printf("3. Use indices instead of pointers for graphs/trees\n");
    printf("4. Fixed-size arrays avoid pointer indirection\n");
    printf("5. Return new pointers instead of pointer-to-pointer params\n");
    printf("6. Wrap complex structures in clear abstractions\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] No triple pointers (***) or deeper
 * [ ] Maximum 2 levels of indirection
 * [ ] Clear data ownership
 * [ ] Simple, flat structures where possible
 * [ ] Index-based alternatives considered
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Use index-based structures
 * [ ] Zero dynamic allocation (static pools)
 * [ ] Document pointer ownership
 * [ ] Provide helper macros for indexing
 */
