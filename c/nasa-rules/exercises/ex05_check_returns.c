/*
 * EXERCISE 5: CHECK RETURN VALUES
 * 
 * Task: Add proper return value checking
 * - Check all non-void function returns
 * - Handle all error cases
 * - Use error codes consistently
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex05_check_returns.c -o ex05
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_LINE_LENGTH 256
#define MAX_BUFFER_SIZE 1024

typedef enum {
    ERR_OK = 0,
    ERR_NULL_POINTER,
    ERR_FILE_OPEN,
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_BUFFER_OVERFLOW,
    ERR_INVALID_DATA
} ErrorCode;

// ============================================
// ❌ BAD CODE TO FIX
// ============================================

/* Problem 1: Ignoring file operation returns */
void bad_copy_file(const char *src, const char *dest) {
    FILE *source = fopen(src, "r");  // Not checked!
    FILE *destination = fopen(dest, "w");  // Not checked!
    
    char buffer[MAX_BUFFER_SIZE];
    fread(buffer, 1, MAX_BUFFER_SIZE, source);  // Return ignored!
    fwrite(buffer, 1, MAX_BUFFER_SIZE, destination);  // Return ignored!
    
    fclose(source);  // Not checked!
    fclose(destination);  // Not checked!
}

/* Problem 2: Ignoring string operation returns */
void bad_string_operations(char *dest, const char *src) {
    strcpy(dest, src);  // No bounds check!
    
    char *token = strtok(dest, ",");  // Could be NULL!
    printf("Token: %s\n", token);  // Crash if NULL!
    
    int value = atoi(src);  // No error checking possible!
    printf("Value: %d\n", value);
}

/* Problem 3: Ignoring memory allocation returns */
int* bad_allocate_array(size_t size) {
    int *array = malloc(size * sizeof(int));  // Could be NULL!
    
    for (size_t i = 0; i < size; i++) {
        array[i] = (int)i;  // Crash if malloc failed!
    }
    
    return array;
}

/* Problem 4: Chaining operations without checks */
void bad_chained_operations(const char *filename) {
    FILE *file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, file);  // file could be NULL!
    
    int value = atoi(line);  // line could be garbage!
    printf("Read value: %d\n", value);
    
    fclose(file);
}

// ============================================
// ✅ YOUR TASK: ADD PROPER ERROR CHECKING
// ============================================

/* TODO: Fix problem 1
 * Requirements:
 * - Check fopen returns
 * - Check fread/fwrite returns
 * - Check fclose returns
 * - Return appropriate error code
 * - Clean up on all error paths
 */
ErrorCode good_copy_file(const char *src, const char *dest) {
    // TODO: Implement with proper error checking
    (void)src;
    (void)dest;
    return ERR_OK;
}

/* TODO: Fix problem 2
 * Requirements:
 * - Use strncpy instead of strcpy
 * - Check strtok return
 * - Use strtol instead of atoi
 * - Check strtol errno
 * - Return error code
 */
ErrorCode good_string_operations(char *dest, size_t dest_size, 
                                  const char *src, int *out_value) {
    // TODO: Implement with proper error checking
    (void)dest;
    (void)dest_size;
    (void)src;
    (void)out_value;
    return ERR_OK;
}

/* TODO: Fix problem 3
 * Requirements:
 * - Check malloc return
 * - Return NULL on failure
 * - Set errno if helpful
 * - Document return value in comment
 */
int* good_allocate_array(size_t size) {
    // TODO: Implement with proper error checking
    // Returns: Pointer to array or NULL on failure
    (void)size;
    return NULL;
}

/* TODO: Fix problem 4
 * Requirements:
 * - Check each operation
 * - Return error code
 * - Early return on errors
 * - Ensure file is closed
 */
ErrorCode good_chained_operations(const char *filename, int *out_value) {
    // TODO: Implement with proper error checking
    (void)filename;
    (void)out_value;
    return ERR_OK;
}

/* TODO: Add helper function
 * Requirements:
 * - Convert string to int safely
 * - Check for conversion errors
 * - Handle overflow
 * - Return success/failure
 */
bool safe_string_to_int(const char *str, int *out_value) {
    // TODO: Implement using strtol
    (void)str;
    (void)out_value;
    return false;
}

/* TODO: Add error reporting function
 * Requirements:
 * - Convert ErrorCode to string
 * - Use switch with all cases
 * - Return descriptive message
 */
const char* error_to_string(ErrorCode error) {
    // TODO: Implement error string conversion
    (void)error;
    return "Unknown error";
}

// ============================================
// ADVANCED: File Line Reader
// ============================================

/* TODO: Read file line by line with error checking
 * Requirements:
 * - Check all file operations
 * - Handle empty files
 * - Handle read errors
 * - Close file on all paths
 * - Return number of lines read or error
 */
ErrorCode read_file_lines(const char *filename, 
                          char lines[][MAX_LINE_LENGTH],
                          size_t max_lines,
                          size_t *lines_read) {
    // TODO: Implement safe line reading
    (void)filename;
    (void)lines;
    (void)max_lines;
    (void)lines_read;
    return ERR_OK;
}

// ============================================
// TEST HARNESS
// ============================================

void test_file_operations(void) {
    printf("Test 1: File Operations\n");
    
    // Create test file
    FILE *test = fopen("test_source.txt", "w");
    if (test != NULL) {
        fprintf(test, "Hello, NASA!\n");
        fclose(test);
    }
    
    printf("  Bad version:\n");
    bad_copy_file("test_source.txt", "test_dest_bad.txt");
    printf("    (no error checking)\n");
    
    printf("  Good version:\n");
    ErrorCode err = good_copy_file("test_source.txt", "test_dest_good.txt");
    printf("    Result: %s\n", error_to_string(err));
    
    // Test with missing file
    err = good_copy_file("nonexistent.txt", "output.txt");
    printf("    Missing file: %s\n\n", error_to_string(err));
}

void test_string_operations(void) {
    printf("Test 2: String Operations\n");
    
    char buffer[64];
    int value;
    
    printf("  Good version with valid input:\n");
    ErrorCode err = good_string_operations(buffer, sizeof(buffer), 
                                          "123,456", &value);
    printf("    Result: %s, Value: %d\n", error_to_string(err), value);
    
    printf("  Good version with invalid input:\n");
    err = good_string_operations(buffer, sizeof(buffer), 
                                 "not_a_number", &value);
    printf("    Result: %s\n\n", error_to_string(err));
}

void test_allocation(void) {
    printf("Test 3: Memory Allocation\n");
    
    printf("  Good version with valid size:\n");
    int *array = good_allocate_array(10);
    if (array != NULL) {
        printf("    Allocation successful\n");
        free(array);
    } else {
        printf("    Allocation failed\n");
    }
    
    printf("  Good version with huge size:\n");
    array = good_allocate_array(SIZE_MAX / sizeof(int));
    if (array != NULL) {
        printf("    Allocation successful (unexpected!)\n");
        free(array);
    } else {
        printf("    Allocation failed (expected)\n");
    }
    printf("\n");
}

void test_chained_operations(void) {
    printf("Test 4: Chained Operations\n");
    
    // Create test file
    FILE *test = fopen("test_number.txt", "w");
    if (test != NULL) {
        fprintf(test, "42\n");
        fclose(test);
    }
    
    int value;
    printf("  Good version with valid file:\n");
    ErrorCode err = good_chained_operations("test_number.txt", &value);
    printf("    Result: %s, Value: %d\n", error_to_string(err), value);
    
    printf("  Good version with missing file:\n");
    err = good_chained_operations("missing.txt", &value);
    printf("    Result: %s\n\n", error_to_string(err));
}

int main(void) {
    printf("EXERCISE 5: CHECK RETURN VALUES\n");
    printf("================================\n\n");
    
    test_file_operations();
    test_string_operations();
    test_allocation();
    test_chained_operations();
    
    printf("✅ Exercise 5 complete!\n");
    printf("\nHints:\n");
    printf("1. Check EVERY non-void function return\n");
    printf("2. Use 'if (ptr == NULL)' after malloc\n");
    printf("3. Use 'if (file == NULL)' after fopen\n");
    printf("4. Use strtol instead of atoi\n");
    printf("5. Use strncpy instead of strcpy\n");
    printf("6. Check errno after strtol\n");
    printf("7. Return error codes, not void\n");
    printf("8. Clean up resources on ALL error paths\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] All fopen returns checked
 * [ ] All fread/fwrite returns checked
 * [ ] All malloc returns checked
 * [ ] String functions used safely
 * [ ] Error codes returned consistently
 * [ ] Resources cleaned up on errors
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Use errno for system call errors
 * [ ] Log errors with details
 * [ ] Add retry logic for transient errors
 * [ ] Create error context struct
 */
