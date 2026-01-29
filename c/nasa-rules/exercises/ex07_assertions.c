/*
 * EXERCISE 7: USE ASSERTIONS
 * 
 * Task: Add comprehensive assertions
 * - Check preconditions
 * - Check postconditions
 * - Check invariants
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex07_assertions.c -o ex07
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define MAX_QUEUE_SIZE 10
#define MAX_NAME_LENGTH 32

// ============================================
// ❌ BAD CODE TO FIX - NO ASSERTIONS
// ============================================

/* Problem 1: No precondition checks */
typedef struct {
    int items[MAX_QUEUE_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} CircularQueue;

void bad_queue_enqueue(CircularQueue *queue, int value) {
    // No checks!
    queue->items[queue->tail] = value;
    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    queue->count++;
}

int bad_queue_dequeue(CircularQueue *queue) {
    // No checks!
    int value = queue->items[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    queue->count--;
    return value;
}

/* Problem 2: No array bounds validation */
void bad_array_operations(int *array, size_t size, size_t index, int value) {
    // No bounds check!
    array[index] = value;
}

/* Problem 3: No invariant checks */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int age;  // Should be 0-150
    double balance;  // Should be >= 0
    bool active;
} Account;

void bad_update_account(Account *account, double amount) {
    // No validation!
    account->balance += amount;
}

/* Problem 4: No pointer validation */
void bad_string_copy(char *dest, const char *src) {
    // No NULL checks!
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// ============================================
// ✅ YOUR TASK: ADD COMPREHENSIVE ASSERTIONS
// ============================================

/* TODO: Fix queue operations
 * Requirements:
 * - Assert queue not NULL
 * - Assert count invariant
 * - Assert queue not full (enqueue)
 * - Assert queue not empty (dequeue)
 * - Assert head/tail in bounds
 */
void good_queue_init(CircularQueue *queue) {
    // TODO: Add assertions and initialize
    (void)queue;
}

void good_queue_enqueue(CircularQueue *queue, int value) {
    // TODO: Add precondition assertions
    // TODO: Perform operation
    // TODO: Add postcondition assertions
    (void)queue;
    (void)value;
}

int good_queue_dequeue(CircularQueue *queue) {
    // TODO: Add precondition assertions
    // TODO: Perform operation
    // TODO: Add postcondition assertions
    (void)queue;
    return 0;
}

bool good_queue_is_full(const CircularQueue *queue) {
    // TODO: Implement with assertion
    (void)queue;
    return false;
}

bool good_queue_is_empty(const CircularQueue *queue) {
    // TODO: Implement with assertion
    (void)queue;
    return false;
}

/* TODO: Fix array operations
 * Requirements:
 * - Assert array not NULL
 * - Assert size > 0
 * - Assert index < size
 * - Assert index valid
 */
void good_array_set(int *array, size_t size, size_t index, int value) {
    // TODO: Add all assertions
    (void)array;
    (void)size;
    (void)index;
    (void)value;
}

int good_array_get(const int *array, size_t size, size_t index) {
    // TODO: Add all assertions
    (void)array;
    (void)size;
    (void)index;
    return 0;
}

/* TODO: Fix account operations
 * Requirements:
 * - Assert account not NULL
 * - Assert age in range [0, 150]
 * - Assert balance >= 0
 * - Assert name not empty
 * - Check invariants before and after
 */
bool good_account_is_valid(const Account *account) {
    // TODO: Check all invariants
    (void)account;
    return false;
}

void good_create_account(Account *account, const char *name, int age) {
    // TODO: Add assertions for parameters
    // TODO: Initialize account
    // TODO: Assert postconditions
    (void)account;
    (void)name;
    (void)age;
}

bool good_update_balance(Account *account, double amount) {
    // TODO: Assert preconditions
    // TODO: Check if operation would violate invariants
    // TODO: Perform operation
    // TODO: Assert postconditions
    (void)account;
    (void)amount;
    return false;
}

/* TODO: Fix string operations
 * Requirements:
 * - Assert dest not NULL
 * - Assert src not NULL
 * - Assert dest_size > 0
 * - Assert dest has enough space
 * - Assert result is null-terminated
 */
void good_string_copy(char *dest, size_t dest_size, const char *src) {
    // TODO: Add all assertions
    (void)dest;
    (void)dest_size;
    (void)src;
}

// ============================================
// ADVANCED: Matrix Operations
// ============================================

#define MAX_MATRIX_SIZE 5

typedef struct {
    int data[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    size_t rows;
    size_t cols;
} Matrix;

/* TODO: Matrix operations with assertions
 * Requirements:
 * - Assert matrix not NULL
 * - Assert dimensions valid (> 0, <= MAX)
 * - Assert row/col indices in bounds
 * - Assert dimensions compatible for operations
 */

void good_matrix_init(Matrix *matrix, size_t rows, size_t cols) {
    // TODO: Add assertions and initialize
    (void)matrix;
    (void)rows;
    (void)cols;
}

void good_matrix_set(Matrix *matrix, size_t row, size_t col, int value) {
    // TODO: Add assertions
    (void)matrix;
    (void)row;
    (void)col;
    (void)value;
}

int good_matrix_get(const Matrix *matrix, size_t row, size_t col) {
    // TODO: Add assertions
    (void)matrix;
    (void)row;
    (void)col;
    return 0;
}

bool good_matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result) {
    // TODO: Assert dimensions compatible (a->cols == b->rows)
    // TODO: Assert result matrix has correct dimensions
    // TODO: Perform multiplication with bounds checks
    (void)a;
    (void)b;
    (void)result;
    return false;
}

// ============================================
// TEST HARNESS
// ============================================

void test_queue_operations(void) {
    printf("Test 1: Queue Operations\n");
    
    CircularQueue queue = {0};
    good_queue_init(&queue);
    
    printf("  Enqueue 3 items...\n");
    good_queue_enqueue(&queue, 10);
    good_queue_enqueue(&queue, 20);
    good_queue_enqueue(&queue, 30);
    
    printf("  Dequeue: %d\n", good_queue_dequeue(&queue));
    printf("  Dequeue: %d\n", good_queue_dequeue(&queue));
    
    printf("  Queue operations successful\n\n");
    
    // Uncomment to test assertions:
    // good_queue_enqueue(NULL, 10);  // Should assert
    // for (int i = 0; i < MAX_QUEUE_SIZE + 1; i++) {
    //     good_queue_enqueue(&queue, i);  // Should assert on overflow
    // }
}

void test_array_operations(void) {
    printf("Test 2: Array Operations\n");
    
    int array[5] = {0};
    
    printf("  Setting array values...\n");
    good_array_set(array, 5, 0, 10);
    good_array_set(array, 5, 4, 50);
    
    printf("  Getting values: %d, %d\n", 
           good_array_get(array, 5, 0),
           good_array_get(array, 5, 4));
    
    // Uncomment to test assertions:
    // good_array_set(array, 5, 10, 100);  // Should assert (out of bounds)
    // good_array_set(NULL, 5, 0, 100);  // Should assert (NULL pointer)
    
    printf("\n");
}

void test_account_operations(void) {
    printf("Test 3: Account Operations\n");
    
    Account account = {0};
    good_create_account(&account, "Alice", 30);
    
    printf("  Account created: %s, age %d\n", account.name, account.age);
    printf("  Initial balance: %.2f\n", account.balance);
    
    printf("  Deposit 100.00...\n");
    good_update_balance(&account, 100.0);
    printf("  Balance: %.2f\n", account.balance);
    
    printf("  Withdraw 30.00...\n");
    good_update_balance(&account, -30.0);
    printf("  Balance: %.2f\n", account.balance);
    
    // Uncomment to test assertions:
    // good_update_balance(&account, -1000.0);  // Should fail (negative balance)
    // good_create_account(&account, "", 30);  // Should assert (empty name)
    // good_create_account(&account, "Bob", 200);  // Should assert (invalid age)
    
    printf("\n");
}

void test_matrix_operations(void) {
    printf("Test 4: Matrix Operations\n");
    
    Matrix a = {0}, b = {0}, result = {0};
    
    good_matrix_init(&a, 2, 3);
    good_matrix_init(&b, 3, 2);
    good_matrix_init(&result, 2, 2);
    
    printf("  Matrices initialized\n");
    
    // Set some values
    good_matrix_set(&a, 0, 0, 1);
    good_matrix_set(&a, 0, 1, 2);
    good_matrix_set(&a, 0, 2, 3);
    
    printf("  Matrix operations successful\n");
    
    // Uncomment to test assertions:
    // good_matrix_set(&a, 10, 10, 99);  // Should assert (out of bounds)
    // good_matrix_init(&a, 0, 3);  // Should assert (invalid dimensions)
    // good_matrix_init(&a, 10, 10);  // Should assert (exceeds MAX)
    
    printf("\n");
}

int main(void) {
    printf("EXERCISE 7: USE ASSERTIONS\n");
    printf("==========================\n\n");
    
    test_queue_operations();
    test_array_operations();
    test_account_operations();
    test_matrix_operations();
    
    printf("✅ Exercise 7 complete!\n");
    printf("\nHints:\n");
    printf("1. Assert preconditions at function start\n");
    printf("2. Assert postconditions before return\n");
    printf("3. Assert invariants before and after operations\n");
    printf("4. Use assert(ptr != NULL) for pointers\n");
    printf("5. Use assert(index < size) for array access\n");
    printf("6. Assert valid ranges for values\n");
    printf("7. Assertions document assumptions\n");
    printf("8. Compile with -DNDEBUG to disable in production\n");
    
    printf("\nTo test assertions, uncomment the test code and run.\n");
    printf("Failed assertions will abort the program.\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] All pointers checked with assert
 * [ ] All array indices checked
 * [ ] All preconditions asserted
 * [ ] All postconditions asserted
 * [ ] Invariants checked
 * [ ] Valid ranges enforced
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Custom assert macros with messages
 * [ ] Static assertions for compile-time checks
 * [ ] Comprehensive invariant functions
 * [ ] Document all assumptions
 */
