/*
 * EXERCISE 10: STATIC ANALYSIS
 * 
 * Task: Write code that passes static analysis
 * - Use clang --analyze
 * - Use cppcheck
 * - Fix all analysis warnings
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex10_static_analysis.c -o ex10
 * Analyze: clang --analyze ex10_static_analysis.c
 *          cppcheck --enable=all ex10_static_analysis.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_BUFFER 256

// ============================================
// ❌ BAD CODE TO FIX - STATIC ANALYSIS ERRORS
// ============================================

/* Problem 1: Null pointer dereference */
void bad_null_deref(int *ptr) {
    *ptr = 42;  // Static analysis: ptr could be NULL
}

/* Problem 2: Memory leak */
void bad_memory_leak(size_t size) {
    int *data = malloc(size * sizeof(int));
    if (data == NULL) {
        return;  // Leak: forgot to free
    }
    
    for (size_t i = 0; i < size; i++) {
        data[i] = (int)i;
    }
    
    if (size > 100) {
        return;  // Leak: forgot to free on early return
    }
    
    free(data);
}

/* Problem 3: Buffer overflow */
void bad_buffer_overflow(const char *src) {
    char dest[10];
    strcpy(dest, src);  // Static analysis: potential overflow
    printf("%s\n", dest);
}

/* Problem 4: Use after free */
void bad_use_after_free(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    
    printf("%d\n", *ptr);  // Use after free!
}

/* Problem 5: Double free */
void bad_double_free(int *ptr) {
    free(ptr);
    free(ptr);  // Double free!
}

/* Problem 6: Uninitialized memory read */
void bad_uninitialized_read(void) {
    int *array = malloc(10 * sizeof(int));
    
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);  // Reading uninitialized memory
    }
    
    free(array);
}

/* Problem 7: Resource leak (file) */
void bad_file_leak(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return;
    }
    
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        return;  // Leak: file not closed
    }
    
    fclose(file);
}

/* Problem 8: Dead code */
int bad_dead_code(int x) {
    if (x > 0) {
        return 1;
    } else {
        return 0;
    }
    
    return -1;  // Dead code: never reached
}

/* Problem 9: Division by zero */
int bad_division(int a, int b) {
    return a / b;  // Could be division by zero
}

/* Problem 10: Array out of bounds */
void bad_array_bounds(void) {
    int array[10];
    for (int i = 0; i <= 10; i++) {  // Off by one!
        array[i] = i;
    }
}

// ============================================
// ✅ YOUR TASK: FIX STATIC ANALYSIS ISSUES
// ============================================

/* TODO: Fix problem 1 - Null pointer check */
bool good_safe_assign(int *ptr, int value) {
    // TODO: Check ptr before dereferencing
    (void)ptr;
    (void)value;
    return false;
}

/* TODO: Fix problem 2 - No memory leaks */
bool good_no_leak(size_t size) {
    // TODO: Ensure free() called on all paths
    // Use goto cleanup pattern or ensure blocks
    (void)size;
    return false;
}

/* TODO: Fix problem 3 - Safe string copy */
bool good_safe_strcpy(char *dest, size_t dest_size, const char *src) {
    // TODO: Use strncpy or bounds checking
    (void)dest;
    (void)dest_size;
    (void)src;
    return false;
}

/* TODO: Fix problem 4 - No use after free */
void good_safe_free(int **ptr) {
    // TODO: Set pointer to NULL after free
    (void)ptr;
}

/* TODO: Fix problem 5 - No double free */
void good_no_double_free(int **ptr) {
    // TODO: Check and null pointer after free
    (void)ptr;
}

/* TODO: Fix problem 6 - Initialize memory */
int* good_initialized_malloc(size_t count) {
    // TODO: Use calloc or initialize after malloc
    (void)count;
    return NULL;
}

/* TODO: Fix problem 7 - No resource leak */
bool good_file_no_leak(const char *filename, char *buffer, size_t buffer_size) {
    // TODO: Ensure file closed on all paths
    (void)filename;
    (void)buffer;
    (void)buffer_size;
    return false;
}

/* TODO: Fix problem 8 - Remove dead code */
int good_no_dead_code(int x) {
    // TODO: Remove unreachable code
    (void)x;
    return 0;
}

/* TODO: Fix problem 9 - Check division */
bool good_safe_divide(int a, int b, int *result) {
    // TODO: Check for zero before division
    (void)a;
    (void)b;
    (void)result;
    return false;
}

/* TODO: Fix problem 10 - Correct array bounds */
void good_array_bounds(int *array, size_t size) {
    // TODO: Use correct loop bounds
    (void)array;
    (void)size;
}

// ============================================
// ADVANCED: Complex Function with Multiple Issues
// ============================================

/* TODO: Fix all static analysis issues */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynamicArray;

void bad_complex_function(DynamicArray *array, const char *filename) {
    // Issue 1: No null check on array
    FILE *file = fopen(filename, "r");
    // Issue 2: No null check on file
    
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int value = atoi(line);
        
        // Issue 3: No capacity check
        array->data[array->size] = value;
        array->size++;
    }
    
    // Issue 4: File not closed on all paths
    if (array->size == 0) {
        return;  // Leak!
    }
    
    fclose(file);
}

DynamicArray* good_complex_function(const char *filename) {
    // TODO: Fix all issues
    // - Check all pointers
    // - No memory leaks
    // - No buffer overflows
    // - Close resources on all paths
    (void)filename;
    return NULL;
}

// ============================================
// STATIC ANALYSIS HELPERS
// ============================================

/* Use these patterns for cleaner code */

// Pattern 1: RAII-like cleanup with goto
bool good_cleanup_pattern(const char *file1, const char *file2) {
    FILE *f1 = NULL;
    FILE *f2 = NULL;
    char *buffer = NULL;
    bool success = false;
    
    f1 = fopen(file1, "r");
    if (f1 == NULL) {
        goto cleanup;
    }
    
    f2 = fopen(file2, "w");
    if (f2 == NULL) {
        goto cleanup;
    }
    
    buffer = malloc(MAX_BUFFER);
    if (buffer == NULL) {
        goto cleanup;
    }
    
    // Do work...
    success = true;
    
cleanup:
    if (buffer != NULL) {
        free(buffer);
    }
    if (f2 != NULL) {
        fclose(f2);
    }
    if (f1 != NULL) {
        fclose(f1);
    }
    
    return success;
}

// Pattern 2: Safe pointer setter
void safe_free_and_null(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

// Pattern 3: Bounds-checked array access
bool safe_array_set(int *array, size_t size, size_t index, int value) {
    if (array == NULL || index >= size) {
        return false;
    }
    array[index] = value;
    return true;
}

// ============================================
// TEST HARNESS
// ============================================

void test_pointer_safety(void) {
    printf("Test 1: Pointer Safety\n");
    
    int value = 0;
    bool success = good_safe_assign(&value, 42);
    printf("  Assign result: %s, value: %d\n", 
           success ? "success" : "fail", value);
    
    success = good_safe_assign(NULL, 42);
    printf("  Assign to NULL: %s\n", 
           success ? "success" : "fail (expected)");
    printf("\n");
}

void test_memory_safety(void) {
    printf("Test 2: Memory Safety\n");
    
    printf("  Allocation with no leaks...\n");
    good_no_leak(50);
    
    printf("  Initialized malloc...\n");
    int *array = good_initialized_malloc(10);
    if (array != NULL) {
        printf("    First element: %d (should be 0)\n", array[0]);
        free(array);
    }
    
    printf("\n");
}

void test_string_safety(void) {
    printf("Test 3: String Safety\n");
    
    char buffer[20];
    bool success = good_safe_strcpy(buffer, sizeof(buffer), "Hello");
    printf("  Short string: %s (%s)\n", 
           success ? buffer : "fail", 
           success ? "success" : "fail");
    
    success = good_safe_strcpy(buffer, sizeof(buffer), 
                               "This is a very long string that won't fit");
    printf("  Long string: %s\n", 
           success ? "copied" : "rejected (expected)");
    printf("\n");
}

void test_division_safety(void) {
    printf("Test 4: Division Safety\n");
    
    int result;
    bool success = good_safe_divide(10, 2, &result);
    printf("  10 / 2 = %d (%s)\n", result, 
           success ? "success" : "fail");
    
    success = good_safe_divide(10, 0, &result);
    printf("  10 / 0 = %s\n", 
           success ? "unexpected" : "rejected (expected)");
    printf("\n");
}

int main(void) {
    printf("EXERCISE 10: STATIC ANALYSIS\n");
    printf("=============================\n\n");
    
    printf("Run static analysis with:\n");
    printf("  clang --analyze ex10_static_analysis.c\n");
    printf("  cppcheck --enable=all ex10_static_analysis.c\n\n");
    
    test_pointer_safety();
    test_memory_safety();
    test_string_safety();
    test_division_safety();
    
    printf("✅ Exercise 10 complete!\n");
    printf("\nStatic Analysis Checklist:\n");
    printf("[ ] No null pointer dereferences\n");
    printf("[ ] No memory leaks\n");
    printf("[ ] No buffer overflows\n");
    printf("[ ] No use after free\n");
    printf("[ ] No double free\n");
    printf("[ ] No uninitialized reads\n");
    printf("[ ] No resource leaks\n");
    printf("[ ] No dead code\n");
    printf("[ ] No division by zero\n");
    printf("[ ] No array out of bounds\n");
    
    printf("\nTools:\n");
    printf("  - Clang Static Analyzer\n");
    printf("  - Cppcheck\n");
    printf("  - Coverity (commercial)\n");
    printf("  - PVS-Studio (commercial)\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] Passes clang --analyze with no warnings
 * [ ] Passes cppcheck with no errors
 * [ ] All pointers checked before use
 * [ ] No memory leaks
 * [ ] No resource leaks
 * [ ] No buffer overflows
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Use Valgrind for runtime checks
 * [ ] Add sanitizer flags (-fsanitize=address)
 * [ ] Document analysis results
 * [ ] Create analysis script
 */
