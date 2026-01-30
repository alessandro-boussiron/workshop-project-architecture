/*
 * EXERCISE 1: CONTROL FLOW REFACTORING
 * 
 * Task: Fix this code to comply with NASA Rule 1
 * - Remove all goto statements
 * - Use structured control flow
 * - Add proper error handling
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex01_control_flow.c -o ex01
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_COMMANDS 10

// ============================================
// ❌ BAD CODE TO FIX
// ============================================

/* Problem 1: goto-based command processor */
int bad_process_commands(char **commands, int count) {
    int i = 0;
    
start:
    if (i >= count) goto end;
    
    if (strcmp(commands[i], "START") == 0) {
        printf("Starting...\n");
        i++;
        goto start;
    }
    
    if (strcmp(commands[i], "STOP") == 0) {
        printf("Stopping...\n");
        goto end;
    }
    
    if (strcmp(commands[i], "PAUSE") == 0) {
        printf("Pausing...\n");
        i++;
        goto start;
    }
    
    printf("Unknown command: %s\n", commands[i]);
    i++;
    goto start;
    
end:
    return i;
}

/* Problem 2: goto for error handling */
int bad_initialize_system(const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL)
        goto error;
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL)
        goto cleanup_file;
    
    // More initialization...
    
    fclose(file);
    return 0;

cleanup_file:
    fclose(file);
error:
    return -1;
}

/* Problem 3: Recursive function */
int bad_recursive_factorial(int n) {
    if (n <= 1)
        return 1;
    return n * bad_recursive_factorial(n - 1);
}

// ============================================
// ✅ YOUR TASK: FIX THE CODE BELOW
// ============================================

/* TODO: Fix problem 1
 * Requirements:
 * - No goto
 * - Use for loop or while loop
 * - Use switch or if-else for commands
 * - Return error code if command unknown
 */
int good_process_commands(char **commands, int count) {
    for (unsigned int i = 0; i < count; i++){
        if (!strcmp(commands[i], "START"))
            printf("Starting...\n");
        
        if (!strcmp(commands[i], "STOP") == 0) {
            printf("Stopping...\n");
            return 1;
        }
        
        if (!strcmp(commands[i], "PAUSE") == 0)
            printf("Pausing...\n");
        printf("Unknown command: %s\n", commands[i]);
    }
    return 0;
}

/* TODO: Fix problem 2
 * Requirements:
 * - No goto
 * - Use early returns for errors
 * - Ensure file is always closed
 */
int good_initialize_system(const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL)
        return -1;;
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

/* TODO: Fix problem 3
 * Requirements:
 * - No recursion
 * - Use iterative approach
 * - Bounded loop
 */
int good_iterative_factorial(int n) {
    int value = 1;
    for (int i = 1; i <= n; i++)
        value *= i;
    return value;
}

// ============================================
// TEST HARNESS
// ============================================

void test_command_processor(void) {
    printf("Test: Command Processor\n");
    
    char *commands[] = {"START", "PAUSE", "STOP"};
    
    printf("  Bad version: ");
    bad_process_commands(commands, 3);
    
    printf("  Good version: ");
    int result = good_process_commands(commands, 3);
    printf("  Result: %d\n\n", result);
}

void test_initialization(void) {
    printf("Test: System Initialization\n");
    
    printf("  Bad version (with goto): %d\n", bad_initialize_system("config.txt"));
    printf("  Good version (no goto): %d\n\n", good_initialize_system("config.txt"));
}

void test_factorial(void) {
    printf("Test: Factorial\n");
    
    printf("  Bad (recursive): 5! = %d\n", bad_recursive_factorial(5));
    printf("  Good (iterative): 5! = %d\n\n", good_iterative_factorial(5));
}

int main(void) {
    printf("EXERCISE 1: CONTROL FLOW REFACTORING\n");
    printf("=====================================\n\n");
    
    test_command_processor();
    test_initialization();
    test_factorial();
    
    printf("✅ Exercise 1 complete!\n");
    printf("\nHints:\n");
    printf("1. Use 'for' loop with bound checks\n");
    printf("2. Use 'switch' statement for commands\n");
    printf("3. Return early for errors\n");
    printf("4. Replace recursion with 'for' loop\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] No goto statements
 * [ ] No recursion
 * [ ] Clear control flow
 * [ ] Proper error handling
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Use enums for command types
 * [ ] Add bounds checking
 * [ ] Document your changes
 */
