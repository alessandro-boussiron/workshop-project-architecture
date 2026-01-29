/*
 * EXERCISE 2: FIXED LOOP BOUNDS
 * 
 * Task: Add fixed upper bounds to all loops
 * - Define MAX constants
 * - Ensure all loops terminate
 * - Add timeout mechanisms
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex02_loop_bounds.c -o ex02
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// TODO: Add your MAX_ defines here
// #define MAX_INPUT_SIZE ???
// #define MAX_ARRAY_SIZE ???
// #define MAX_WAIT_CYCLES ???

// ============================================
// ❌ BAD CODE TO FIX
// ============================================

/* Problem 1: Unbounded string parsing */
void bad_parse_input(char *input) {
    int i = 0;
    while (input[i] != '\0') {  // What if no null terminator?
        printf("%c", input[i]);
        i++;
    }
    printf("\n");
}

/* Problem 2: Search without bound */
int bad_find_value(int *array, int value) {
    int i = 0;
    while (array[i] != value) {  // What if value not in array?
        i++;
    }
    return i;
}

/* Problem 3: Waiting without timeout */
void bad_wait_for_ready(volatile bool *ready) {
    while (!*ready) {  // Could wait forever!
        // Busy wait
    }
}

/* Problem 4: Processing until sentinel */
int bad_process_stream(uint8_t *stream) {
    int count = 0;
    while (stream[count] != 0xFF) {  // What if no sentinel?
        printf("Byte: 0x%02X\n", stream[count]);
        count++;
    }
    return count;
}

/* Problem 5: Dynamic bound based on data */
void bad_nested_loops(int n) {
    for (int i = 0; i < n; i++) {  // n unbounded
        int j = 0;
        while (j < i * 2) {  // Bound changes with i
            printf("%d,%d ", i, j);
            j++;
        }
        printf("\n");
    }
}

// ============================================
// ✅ YOUR TASK: FIX THE CODE BELOW
// ============================================

/* TODO: Fix problem 1
 * Requirements:
 * - Add MAX_INPUT_SIZE constant
 * - Loop bounded by MAX_INPUT_SIZE
 * - Check for null terminator within bound
 * - Return actual length
 */
size_t good_parse_input(char *input) {
    // TODO: Implement with fixed bound
    (void)input;
    return 0;  // Placeholder
}

/* TODO: Fix problem 2
 * Requirements:
 * - Add MAX_ARRAY_SIZE constant
 * - Search only up to max size
 * - Return -1 if not found
 * - Pass size as parameter
 */
int good_find_value(int *array, size_t size, int value) {
    // TODO: Implement with fixed bound
    (void)array;
    (void)size;
    (void)value;
    return -1;  // Placeholder
}

/* TODO: Fix problem 3
 * Requirements:
 * - Add MAX_WAIT_CYCLES constant
 * - Exit after max iterations
 * - Return success/failure
 */
bool good_wait_for_ready(volatile bool *ready) {
    // TODO: Implement with timeout
    (void)ready;
    return false;  // Placeholder
}

/* TODO: Fix problem 4
 * Requirements:
 * - Add MAX_STREAM_SIZE constant
 * - Process up to maximum
 * - Don't rely on sentinel
 * - Return bytes processed
 */
int good_process_stream(uint8_t *stream, size_t max_size) {
    // TODO: Implement with fixed bound
    (void)stream;
    (void)max_size;
    return 0;  // Placeholder
}

/* TODO: Fix problem 5
 * Requirements:
 * - Add MAX_OUTER and MAX_INNER constants
 * - Both loops bounded by constants
 * - Clamp n to maximum
 */
void good_nested_loops(int n) {
    // TODO: Implement with fixed bounds
    (void)n;
}

// ============================================
// TEST HARNESS
// ============================================

void test_parse_input(void) {
    printf("Test 1: Parse Input\n");
    
    char input[] = "Hello, NASA!";
    printf("  Bad version: ");
    bad_parse_input(input);
    
    printf("  Good version: ");
    size_t len = good_parse_input(input);
    printf("  (length: %zu)\n\n", len);
}

void test_find_value(void) {
    printf("Test 2: Find Value\n");
    
    int array[] = {10, 20, 30, 40, 50};
    
    printf("  Bad version: Finding 30...\n");
    // Can't safely call bad version if value not present
    
    printf("  Good version: Finding 30...\n");
    int pos = good_find_value(array, 5, 30);
    printf("    Position: %d\n", pos);
    
    printf("  Good version: Finding 999 (not present)...\n");
    pos = good_find_value(array, 5, 999);
    printf("    Position: %d\n\n", pos);
}

void test_wait_for_ready(void) {
    printf("Test 3: Wait for Ready\n");
    
    volatile bool ready = false;
    
    printf("  Good version: Waiting (will timeout)...\n");
    bool success = good_wait_for_ready(&ready);
    printf("    Success: %s\n\n", success ? "true" : "false");
}

void test_process_stream(void) {
    printf("Test 4: Process Stream\n");
    
    uint8_t stream[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0xFF};
    
    printf("  Bad version:\n");
    bad_process_stream(stream);
    
    printf("  Good version:\n");
    int count = good_process_stream(stream, 6);
    printf("    Processed %d bytes\n\n", count);
}

void test_nested_loops(void) {
    printf("Test 5: Nested Loops\n");
    
    printf("  Bad version (n=5):\n");
    bad_nested_loops(5);
    
    printf("  Good version (n=5):\n");
    good_nested_loops(5);
    printf("\n");
}

int main(void) {
    printf("EXERCISE 2: FIXED LOOP BOUNDS\n");
    printf("==============================\n\n");
    
    test_parse_input();
    test_find_value();
    test_wait_for_ready();
    test_process_stream();
    test_nested_loops();
    
    printf("✅ Exercise 2 complete!\n");
    printf("\nHints:\n");
    printf("1. Define MAX constants at top of file\n");
    printf("2. Use 'for' loops with constant bounds\n");
    printf("3. Add 'count' variable for iterations\n");
    printf("4. Check condition AND count < MAX\n");
    printf("5. Return early if bound reached\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] All MAX_ constants defined
 * [ ] No unbounded loops
 * [ ] All loops use constant or parameter bounds
 * [ ] Proper return values for timeout cases
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Add timing measurements
 * [ ] Document worst-case complexity
 * [ ] Add assertions for bounds
 */
