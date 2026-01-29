/*
 * EXERCISE 9: ZERO COMPILER WARNINGS
 * 
 * Task: Fix all compiler warnings
 * - Compile with -Wall -Wextra -Werror
 * - Initialize all variables
 * - Handle all enum cases
 * - Fix type mismatches
 * 
 * Compile: gcc -Wall -Wextra -Werror -pedantic -std=c11 ex09_warnings.c -o ex09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ============================================
// ❌ BAD CODE TO FIX - GENERATES WARNINGS
// ============================================

/* Warning 1: Uninitialized variable */
int bad_uninitialized(void) {
    int x;  // Warning: may be uninitialized
    return x * 2;
}

/* Warning 2: Unused variable */
void bad_unused_variable(void) {
    int used = 10;
    int unused = 20;  // Warning: unused
    printf("%d\n", used);
}

/* Warning 3: Unused parameter */
void bad_unused_parameter(int x, int y) {
    // Warning: parameter 'y' unused
    printf("%d\n", x);
}

/* Warning 4: Missing return */
int bad_missing_return(int x) {
    if (x > 0) {
        return x;
    }
    // Warning: control reaches end of non-void function
}

/* Warning 5: Signed/unsigned comparison */
void bad_sign_comparison(void) {
    int signed_val = -1;
    size_t unsigned_val = 10;
    
    if (signed_val < unsigned_val) {  // Warning: signed/unsigned comparison
        printf("Less\n");
    }
}

/* Warning 6: Implicit conversion */
void bad_implicit_conversion(void) {
    double d = 3.14159;
    int i = d;  // Warning: implicit conversion loses precision
    printf("%d\n", i);
}

/* Warning 7: Missing enum case */
typedef enum {
    STATE_INIT,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED
} State;

void bad_switch_incomplete(State state) {
    switch (state) {
        case STATE_INIT:
            printf("Init\n");
            break;
        case STATE_RUNNING:
            printf("Running\n");
            break;
        // Warning: enumeration values not handled
    }
}

/* Warning 8: Format string mismatch */
void bad_format_string(void) {
    size_t size = 100;
    int value = 42;
    
    printf("Size: %d\n", size);  // Warning: format specifies type 'int' but argument has type 'size_t'
    printf("Value: %zu\n", value);  // Warning: format specifies type 'size_t' but argument has type 'int'
}

/* Warning 9: Pointer arithmetic on void* */
void bad_void_pointer_arithmetic(void *ptr, size_t offset) {
    void *new_ptr = ptr + offset;  // Warning: pointer arithmetic on void*
    printf("%p\n", new_ptr);
}

/* Warning 10: Array subscript type */
void bad_array_subscript(void) {
    int array[10];
    char idx = 5;
    
    printf("%d\n", array[idx]);  // Warning: array subscript is of type 'char'
}

/* Warning 11: Function declared implicitly */
// Warning: implicit declaration of function (if not declared)
void bad_implicit_declaration(void) {
    // some_undeclared_function();  // Would cause warning
}

/* Warning 12: Comparison of constant */
void bad_constant_comparison(int x) {
    if (x = 5) {  // Warning: using assignment as condition
        printf("Equal\n");
    }
}

// ============================================
// ✅ YOUR TASK: FIX ALL WARNINGS
// ============================================

/* TODO: Fix warning 1 - Uninitialized variable */
int good_initialized(void) {
    // TODO: Initialize variable
    return 0;
}

/* TODO: Fix warning 2 - Unused variable */
void good_no_unused(void) {
    // TODO: Remove unused variable or mark as unused
}

/* TODO: Fix warning 3 - Unused parameter */
void good_unused_parameter(int x, int y) {
    // TODO: Use (void)y; to mark as intentionally unused
    // Or use __attribute__((unused)) in GCC
    (void)x;
    (void)y;
}

/* TODO: Fix warning 4 - Missing return */
int good_all_paths_return(int x) {
    // TODO: Ensure all code paths return a value
    (void)x;
    return 0;
}

/* TODO: Fix warning 5 - Signed/unsigned comparison */
void good_sign_comparison(void) {
    // TODO: Cast to same type or restructure comparison
}

/* TODO: Fix warning 6 - Implicit conversion */
void good_explicit_conversion(void) {
    // TODO: Use explicit cast
}

/* TODO: Fix warning 7 - Missing enum case */
void good_switch_complete(State state) {
    // TODO: Handle all enum cases or add default
    (void)state;
}

/* TODO: Fix warning 8 - Format string mismatch */
void good_format_string(void) {
    // TODO: Use correct format specifiers
    // %zu for size_t
    // %d for int
    // %ld for long
    // %p for pointers
}

/* TODO: Fix warning 9 - Void pointer arithmetic */
void good_pointer_arithmetic(void *ptr, size_t offset) {
    // TODO: Cast to char* or uint8_t* for arithmetic
    (void)ptr;
    (void)offset;
}

/* TODO: Fix warning 10 - Array subscript type */
void good_array_subscript(void) {
    // TODO: Use size_t or int for array index
}

/* TODO: Fix warning 12 - Assignment in condition */
void good_comparison(int x) {
    // TODO: Use == for comparison, not =
    (void)x;
}

// ============================================
// COMPREHENSIVE FIX EXAMPLE
// ============================================

/* TODO: Fix all warnings in this function */
typedef enum {
    CMD_START,
    CMD_STOP,
    CMD_RESET,
    CMD_STATUS
} Command;

typedef struct {
    char *name;
    int id;
    double value;
} SensorData;

void bad_complex_function(Command cmd, size_t count, void *data) {
    // Multiple warnings to fix!
    
    int result;  // Uninitialized
    int unused_var = 10;  // Unused
    
    switch (cmd) {
        case CMD_START:
            printf("Starting\n");
            break;
        case CMD_STOP:
            printf("Stopping\n");
            break;
        // Missing cases!
    }
    
    if (result = 0) {  // Assignment in condition
        printf("Zero\n");
    }
    
    for (int i = -1; i < count; i++) {  // Signed/unsigned comparison
        SensorData *sensors = data;  // Unchecked cast
        printf("Sensor: %d\n", sensors[i].id);
    }
    
    printf("Count: %d\n", count);  // Wrong format specifier
}

void good_complex_function(Command cmd, size_t count, void *data) {
    // TODO: Fix all warnings in bad_complex_function
    (void)cmd;
    (void)count;
    (void)data;
}

// ============================================
// TEST HARNESS
// ============================================

void test_basic_fixes(void) {
    printf("Test 1: Basic Warning Fixes\n");
    
    printf("  Initialized: %d\n", good_initialized());
    good_no_unused();
    good_unused_parameter(1, 2);
    printf("  All paths return: %d\n", good_all_paths_return(5));
    
    printf("\n");
}

void test_type_fixes(void) {
    printf("Test 2: Type Warning Fixes\n");
    
    good_sign_comparison();
    good_explicit_conversion();
    good_format_string();
    
    printf("\n");
}

void test_enum_fixes(void) {
    printf("Test 3: Enum Warning Fixes\n");
    
    good_switch_complete(STATE_INIT);
    good_switch_complete(STATE_RUNNING);
    good_switch_complete(STATE_PAUSED);
    good_switch_complete(STATE_STOPPED);
    
    printf("\n");
}

void test_pointer_fixes(void) {
    printf("Test 4: Pointer Warning Fixes\n");
    
    int dummy = 0;
    good_pointer_arithmetic(&dummy, 10);
    good_array_subscript();
    
    printf("\n");
}

int main(void) {
    printf("EXERCISE 9: ZERO COMPILER WARNINGS\n");
    printf("===================================\n\n");
    
    printf("Compile with: gcc -Wall -Wextra -Werror -pedantic -std=c11\n\n");
    
    test_basic_fixes();
    test_type_fixes();
    test_enum_fixes();
    test_pointer_fixes();
    
    printf("✅ Exercise 9 complete!\n");
    printf("\nCommon Warnings & Fixes:\n");
    printf("1. Uninitialized: Initialize all variables\n");
    printf("2. Unused: Remove or mark with (void)var;\n");
    printf("3. Missing return: Add return to all paths\n");
    printf("4. Sign mismatch: Cast or use same types\n");
    printf("5. Implicit cast: Use explicit cast\n");
    printf("6. Missing enum: Handle all cases or add default\n");
    printf("7. Format string: Use %%zu for size_t, %%d for int\n");
    printf("8. Void pointer: Cast to char* for arithmetic\n");
    printf("9. Assignment in if: Use == not =\n");
    
    printf("\nCompiler flags:\n");
    printf("  -Wall: Enable all warnings\n");
    printf("  -Wextra: Enable extra warnings\n");
    printf("  -Werror: Treat warnings as errors\n");
    printf("  -pedantic: Strict ISO C compliance\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] Compiles with -Wall -Wextra -Werror
 * [ ] No uninitialized variables
 * [ ] No unused variables
 * [ ] All enum cases handled
 * [ ] Correct format specifiers
 * [ ] No implicit conversions
 * [ ] All code paths return
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Compiles with -pedantic
 * [ ] Use __attribute__((unused)) where appropriate
 * [ ] Add compile-time assertions
 * [ ] Document intentional unused parameters
 */
