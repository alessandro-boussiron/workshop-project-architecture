/*
 * NASA POWER OF 10 RULES - Complete Examples
 * 
 * Code examples demonstrating all 10 rules for mission-critical software
 * Compilation: gcc -Wall -Wextra -Werror -pedantic -std=c11 nasa_rules.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// ============================================
// RULE 1: RESTRICT CONTROL FLOW
// No goto, setjmp, longjmp, or indirect recursion
// ============================================

/* ❌ BAD: Using goto */
int bad_goto_example(int value) {
    if (value < 0)
        goto error;
    
    return value * 2;

error:
    return -1;
}

/* ✅ GOOD: Structured control flow */
int good_control_flow(int value) {
    if (value < 0) {
        return -1;
    }
    return value * 2;
}

/* ❌ BAD: Indirect recursion */
void bad_function_a(int n);
void bad_function_b(int n);

void bad_function_a(int n) {
    if (n > 0) {
        bad_function_b(n - 1);
    }
}

void bad_function_b(int n) {
    if (n > 0) {
        bad_function_a(n - 1);  // Indirect recursion
    }
}

/* ✅ GOOD: Iterative approach */
void good_iterative(int n) {
    for (int i = 0; i < n; i++) {
        // Process iteratively
    }
}

// ============================================
// RULE 2: FIXED LOOP BOUNDS
// All loops must have a fixed upper bound
// ============================================

#define MAX_ITERATIONS 100
#define BUFFER_SIZE 256

/* ❌ BAD: Unbounded loop */
void bad_unbounded_loop(void) {
    int counter = 0;
    while (1) {  // Infinite loop
        counter++;
        if (counter > 1000) break;  // Exit condition inside
    }
}

/* ✅ GOOD: Loop with fixed bound */
void good_bounded_loop(void) {
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        // Process with guaranteed termination
    }
}

/* ✅ GOOD: Array processing with fixed bound */
int sum_array(const int *array, size_t size) {
    int sum = 0;
    
    // Ensure we don't exceed maximum allowed
    const size_t safe_size = (size > BUFFER_SIZE) ? BUFFER_SIZE : size;
    
    for (size_t i = 0; i < safe_size; i++) {
        sum += array[i];
    }
    
    return sum;
}

// ============================================
// RULE 3: NO DYNAMIC MEMORY AFTER INIT
// No malloc/free after initialization phase
// ============================================

#define MAX_SENSORS 10

/* ❌ BAD: Dynamic allocation in runtime */
typedef struct {
    int *data;
    size_t size;
} BadBuffer;

BadBuffer* bad_create_buffer(size_t size) {
    BadBuffer *buf = malloc(sizeof(BadBuffer));  // Runtime allocation
    if (buf == NULL) return NULL;
    
    buf->data = malloc(size * sizeof(int));  // More allocation
    buf->size = size;
    return buf;
}

/* ✅ GOOD: Static allocation */
typedef struct {
    int data[BUFFER_SIZE];
    size_t size;
} GoodBuffer;

void good_init_buffer(GoodBuffer *buf) {
    assert(buf != NULL);
    buf->size = 0;
    memset(buf->data, 0, sizeof(buf->data));
}

/* ✅ GOOD: Pre-allocated pool */
typedef struct {
    int id;
    int value;
    bool active;
} Sensor;

typedef struct {
    Sensor sensors[MAX_SENSORS];
    size_t count;
} SensorPool;

static SensorPool sensor_pool = {0};

bool sensor_pool_add(int id, int value) {
    if (sensor_pool.count >= MAX_SENSORS) {
        return false;  // Pool full
    }
    
    Sensor *sensor = &sensor_pool.sensors[sensor_pool.count];
    sensor->id = id;
    sensor->value = value;
    sensor->active = true;
    sensor_pool.count++;
    
    return true;
}

// ============================================
// RULE 4: LIMIT FUNCTION SIZE
// Functions must be < 60 lines (one printed page)
// ============================================

/* ❌ BAD: Function too long (over 60 lines with logic) */
void bad_large_function(int *data, size_t size) {
    // Sort data
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                int temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
    
    // Find mean
    long sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += data[i];
    }
    int mean = (int)(sum / size);
    
    // Find median
    int median = data[size / 2];
    
    // Find mode (would be many more lines...)
    // ... lots more code ...
}

/* ✅ GOOD: Split into small functions */
void swap_integers(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void sort_array(int *data, size_t size) {
    assert(data != NULL);
    assert(size <= BUFFER_SIZE);
    
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                swap_integers(&data[j], &data[j + 1]);
            }
        }
    }
}

int calculate_mean(const int *data, size_t size) {
    assert(data != NULL);
    assert(size > 0);
    
    long sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += data[i];
    }
    
    return (int)(sum / size);
}

int find_median(const int *sorted_data, size_t size) {
    assert(sorted_data != NULL);
    assert(size > 0);
    
    return sorted_data[size / 2];
}

// ============================================
// RULE 5: CHECK RETURN VALUES
// Always check return values of non-void functions
// ============================================

/* ❌ BAD: Ignoring return values */
void bad_unchecked_returns(void) {
    FILE *file = fopen("data.txt", "r");
    fread(NULL, 1, 100, file);  // Unchecked fopen result
    fclose(file);
}

/* ✅ GOOD: Check all return values */
typedef enum {
    STATUS_OK = 0,
    STATUS_FILE_ERROR = 1,
    STATUS_READ_ERROR = 2,
    STATUS_INVALID_DATA = 3
} Status;

Status good_checked_returns(void) {
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file\n");
        return STATUS_FILE_ERROR;
    }
    
    char buffer[256];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
    if (bytes_read == 0 && ferror(file)) {
        fprintf(stderr, "Error: Read failed\n");
        fclose(file);
        return STATUS_READ_ERROR;
    }
    
    int close_result = fclose(file);
    if (close_result != 0) {
        fprintf(stderr, "Warning: File close failed\n");
    }
    
    return STATUS_OK;
}

/* ✅ GOOD: Check memory operations */
bool safe_copy_buffer(char *dest, const char *src, size_t dest_size) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(dest_size > 0);
    
    size_t src_len = strnlen(src, dest_size);
    if (src_len >= dest_size) {
        return false;  // Would overflow
    }
    
    // Use safe string copy
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';  // Ensure null termination
    
    return true;
}

// ============================================
// RULE 6: LIMIT SCOPE
// Declare variables in smallest possible scope
// ============================================

/* ❌ BAD: Wide scope */
void bad_wide_scope(void) {
    int i, j, k;  // Declared at function scope
    int temp;
    int sum;
    
    sum = 0;
    for (i = 0; i < 10; i++) {
        sum += i;
    }
    
    for (j = 0; j < 10; j++) {
        temp = j * 2;  // temp used here
        printf("%d\n", temp);
    }
    
    // k never used but declared
}

/* ✅ GOOD: Minimal scope */
void good_minimal_scope(void) {
    // Variables declared where needed
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += i;
    }
    
    // Different scope, different iteration variable
    for (int i = 0; i < 10; i++) {
        int temp = i * 2;  // Scope limited to loop
        printf("%d\n", temp);
    }
}

// ============================================
// RULE 7: CHECK ASSERTIONS
// Use assertions to check assumptions and invariants
// ============================================

typedef struct {
    int values[10];
    size_t count;  // Invariant: count <= 10
} BoundedArray;

/* ✅ GOOD: Assert preconditions */
void add_value(BoundedArray *array, int value) {
    // Preconditions
    assert(array != NULL);
    assert(array->count < 10);  // Invariant check
    
    array->values[array->count] = value;
    array->count++;
    
    // Postcondition
    assert(array->count <= 10);  // Invariant maintained
}

/* ✅ GOOD: Assert function contracts */
int divide_safe(int numerator, int denominator) {
    // Precondition: denominator must be non-zero
    assert(denominator != 0);
    
    int result = numerator / denominator;
    
    // Postcondition: result * denominator should be close to numerator
    assert(result * denominator <= numerator + abs(denominator));
    
    return result;
}

/* ✅ GOOD: Assert array bounds */
int get_array_element(const int *array, size_t index, size_t array_size) {
    assert(array != NULL);
    assert(index < array_size);  // Bounds check
    assert(array_size > 0);
    
    return array[index];
}

// ============================================
// RULE 8: LIMIT POINTER INDIRECTION
// Maximum 2 levels of indirection (**ptr OK, ***ptr NO)
// ============================================

/* ❌ BAD: 3 levels of indirection */
void bad_triple_pointer(int ***ptr) {
    // Too complex, error-prone
    int value = ***ptr;
}

/* ✅ GOOD: Maximum 2 levels */
typedef struct {
    int value;
} Data;

void good_double_pointer(Data **ptr) {
    if (ptr == NULL || *ptr == NULL) {
        return;
    }
    (*ptr)->value = 42;
}

/* ✅ GOOD: Use structs instead of deep pointers */
typedef struct {
    int *data;
    size_t size;
} DataWrapper;

void process_wrapper(DataWrapper *wrapper) {
    assert(wrapper != NULL);
    assert(wrapper->data != NULL);
    
    for (size_t i = 0; i < wrapper->size; i++) {
        wrapper->data[i] *= 2;
    }
}

// ============================================
// RULE 9: ZERO COMPILER WARNINGS
// Compile with -Wall -Wextra -Werror
// ============================================

/* ❌ BAD: Code that generates warnings */
int bad_warnings(void) {
    int x;  // Warning: may be uninitialized
    return x;
    
    int y = 5;  // Warning: unused variable
}

/* ✅ GOOD: Clean code, no warnings */
int good_no_warnings(void) {
    int x = 0;  // Properly initialized
    return x;
}

/* ✅ GOOD: Handle all enum cases */
typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR
} State;

const char* state_to_string(State state) {
    switch (state) {
        case STATE_IDLE:    return "IDLE";
        case STATE_RUNNING: return "RUNNING";
        case STATE_ERROR:   return "ERROR";
        default:
            assert(0);  // Should never reach
            return "UNKNOWN";
    }
}

// ============================================
// RULE 10: USE STATIC ANALYSIS
// Run static analyzers: Coverity, PC-Lint, Clang Static Analyzer
// ============================================

/*
 * Static analysis detects:
 * - Null pointer dereferences
 * - Buffer overflows
 * - Memory leaks
 * - Dead code
 * - Uninitialized variables
 * - Resource leaks
 */

/* ✅ GOOD: Code that passes static analysis */
typedef struct {
    int sensor_id;
    double temperature;
    uint32_t timestamp;
    bool valid;
} TelemetryData;

Status process_telemetry(TelemetryData *data) {
    // Check preconditions (static analyzer verifies)
    if (data == NULL) {
        return STATUS_INVALID_DATA;
    }
    
    // Validate data
    if (!data->valid) {
        return STATUS_INVALID_DATA;
    }
    
    // Process within safe bounds
    if (data->temperature < -273.15 || data->temperature > 1000.0) {
        data->valid = false;
        return STATUS_INVALID_DATA;
    }
    
    // All paths checked, all resources managed
    return STATUS_OK;
}

// ============================================
// COMPLETE EXAMPLE: Spacecraft Telemetry System
// Applying all 10 rules
// ============================================

#define MAX_TELEMETRY_SAMPLES 100

typedef struct {
    TelemetryData samples[MAX_TELEMETRY_SAMPLES];
    size_t count;
    double running_average;
} TelemetryBuffer;

static TelemetryBuffer telemetry_buffer = {0};  // Rule 3: Static allocation

/* Rule 4: Function < 60 lines */
Status add_telemetry_sample(int sensor_id, double temperature) {
    // Rule 7: Assert preconditions
    assert(sensor_id >= 0);
    
    // Rule 2: Fixed loop bounds check
    if (telemetry_buffer.count >= MAX_TELEMETRY_SAMPLES) {
        return STATUS_INVALID_DATA;
    }
    
    // Rule 6: Minimal scope
    TelemetryData *sample = &telemetry_buffer.samples[telemetry_buffer.count];
    
    // Initialize data
    sample->sensor_id = sensor_id;
    sample->temperature = temperature;
    sample->timestamp = (uint32_t)time(NULL);
    sample->valid = true;
    
    telemetry_buffer.count++;
    
    // Update running average
    recalculate_average();  // Rule 4: Small functions
    
    // Rule 7: Assert postcondition
    assert(telemetry_buffer.count <= MAX_TELEMETRY_SAMPLES);
    
    return STATUS_OK;
}

/* Rule 4: Small, focused function */
void recalculate_average(void) {
    if (telemetry_buffer.count == 0) {
        telemetry_buffer.running_average = 0.0;
        return;
    }
    
    double sum = 0.0;
    // Rule 2: Fixed bound
    for (size_t i = 0; i < telemetry_buffer.count; i++) {
        sum += telemetry_buffer.samples[i].temperature;
    }
    
    telemetry_buffer.running_average = sum / telemetry_buffer.count;
}

/* Rule 4: Small function */
double get_average_temperature(void) {
    return telemetry_buffer.running_average;
}

/* Rule 5: Check all return values */
Status save_telemetry_to_file(const char *filename) {
    assert(filename != NULL);  // Rule 7
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {  // Rule 5: Check return
        return STATUS_FILE_ERROR;
    }
    
    // Rule 2: Fixed bound
    for (size_t i = 0; i < telemetry_buffer.count; i++) {
        TelemetryData *sample = &telemetry_buffer.samples[i];
        
        int result = fprintf(file, "%d,%.2f,%u\n",
                            sample->sensor_id,
                            sample->temperature,
                            sample->timestamp);
        
        if (result < 0) {  // Rule 5: Check fprintf
            fclose(file);
            return STATUS_FILE_ERROR;
        }
    }
    
    if (fclose(file) != 0) {  // Rule 5: Check fclose
        return STATUS_FILE_ERROR;
    }
    
    return STATUS_OK;
}

// ============================================
// MAIN - Demonstration
// ============================================

int main(void) {
    printf("🚀 NASA Power of 10 Rules - Examples\n\n");
    
    // Test Rule 1: Control flow
    printf("Rule 1 - Control Flow:\n");
    int result = good_control_flow(10);
    printf("  Result: %d\n\n", result);
    
    // Test Rule 2: Fixed bounds
    printf("Rule 2 - Fixed Loop Bounds:\n");
    good_bounded_loop();
    printf("  Loop completed with fixed bound\n\n");
    
    // Test Rule 3: Static allocation
    printf("Rule 3 - Static Allocation:\n");
    GoodBuffer buffer;
    good_init_buffer(&buffer);
    printf("  Buffer initialized (statically allocated)\n\n");
    
    // Test Rule 4: Small functions
    printf("Rule 4 - Small Functions:\n");
    int data[] = {5, 2, 8, 1, 9};
    sort_array(data, 5);
    int mean = calculate_mean(data, 5);
    printf("  Mean of sorted array: %d\n\n", mean);
    
    // Test Rule 5: Check returns
    printf("Rule 5 - Check Return Values:\n");
    Status status = good_checked_returns();
    printf("  Status: %d\n\n", status);
    
    // Test Rule 7: Assertions
    printf("Rule 7 - Assertions:\n");
    BoundedArray bounded = {.count = 0};
    add_value(&bounded, 42);
    printf("  Value added with assertion checks\n\n");
    
    // Test complete system
    printf("Complete System Test - Telemetry:\n");
    status = add_telemetry_sample(1, 25.5);
    assert(status == STATUS_OK);  // Rule 7
    
    status = add_telemetry_sample(1, 26.0);
    assert(status == STATUS_OK);
    
    status = add_telemetry_sample(1, 25.8);
    assert(status == STATUS_OK);
    
    double avg = get_average_temperature();
    printf("  Average temperature: %.2f°C\n", avg);
    printf("  Samples collected: %zu\n\n", telemetry_buffer.count);
    
    printf("✅ All rules demonstrated successfully!\n");
    printf("\nCompile with: gcc -Wall -Wextra -Werror -pedantic -std=c11 nasa_rules.c\n");
    
    return 0;
}

/*
 * ============================================
 * SUMMARY: NASA POWER OF 10 RULES
 * ============================================
 *
 * These rules create code that is:
 * ✅ Predictable - No surprises, clear behavior
 * ✅ Testable - Small functions, clear contracts
 * ✅ Verifiable - Static analysis can prove correctness
 * ✅ Safe - Bounds checked, resources managed
 * ✅ Maintainable - Small, focused, well-structured
 *
 * Used in:
 * - Mars rovers (Curiosity, Perseverance)
 * - International Space Station software
 * - Aircraft control systems
 * - Medical devices
 * - Nuclear power plants
 *
 * When to apply:
 * - Mission-critical systems
 * - Safety-critical applications
 * - High-reliability requirements
 * - Long-term maintenance needs
 */
