/*
 * EXERCISE 6: LIMIT VARIABLE SCOPE
 * 
 * Task: Minimize variable scope
 * - Declare variables in smallest scope
 * - Avoid global variables
 * - Use block scope for temporaries
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex06_limit_scope.c -o ex06
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_SAMPLES 100

// ============================================
// ❌ BAD CODE TO FIX - WIDE SCOPE
// ============================================

/* Problem 1: All variables at function scope */
void bad_data_processing(int *data, size_t size) {
    int i, j, k;  // Used in different loops
    int temp;
    int sum;
    double average;
    int max;
    int min;
    int range;
    int median;
    bool found;
    int count;
    
    // Calculate sum
    sum = 0;
    for (i = 0; i < (int)size; i++) {
        sum += data[i];
    }
    
    // Calculate average
    average = (double)sum / size;
    printf("Average: %.2f\n", average);
    
    // Find max
    max = data[0];
    for (i = 1; i < (int)size; i++) {
        if (data[i] > max) {
            max = data[i];
        }
    }
    
    // Find min
    min = data[0];
    for (i = 1; i < (int)size; i++) {
        if (data[i] < min) {
            min = data[i];
        }
    }
    
    // Calculate range
    range = max - min;
    printf("Range: %d\n", range);
    
    // Sort for median (bubble sort)
    for (i = 0; i < (int)size - 1; i++) {
        for (j = 0; j < (int)size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
    
    // Find median
    median = data[size / 2];
    printf("Median: %d\n", median);
    
    // Count values above average
    count = 0;
    for (i = 0; i < (int)size; i++) {
        if (data[i] > average) {
            count++;
        }
    }
    printf("Above average: %d\n", count);
}

/* Problem 2: Global variables */
int g_sensor_id;  // Should be passed as parameter
int g_reading_count;  // Should be part of a structure
double g_last_value;  // Should be local
bool g_calibrated;  // Should be in sensor state

void bad_read_sensor(void) {
    g_reading_count++;
    g_last_value = 25.5 + g_sensor_id;
    
    if (!g_calibrated) {
        printf("Sensor %d not calibrated!\n", g_sensor_id);
    }
}

/* Problem 3: Reusing variable names for different purposes */
void bad_reused_variables(int *array, size_t size) {
    int temp;  // Used for multiple purposes
    
    // First use: sorting
    for (size_t i = 0; i < size - 1; i++) {
        if (array[i] > array[i + 1]) {
            temp = array[i];
            array[i] = array[i + 1];
            array[i + 1] = temp;
        }
    }
    
    // Second use: calculation
    temp = 0;
    for (size_t i = 0; i < size; i++) {
        temp += array[i];
    }
    printf("Sum: %d\n", temp);
    
    // Third use: comparison
    temp = array[0];
    for (size_t i = 1; i < size; i++) {
        if (array[i] < temp) {
            temp = array[i];
        }
    }
    printf("Min: %d\n", temp);
}

// ============================================
// ✅ YOUR TASK: FIX VARIABLE SCOPE
// ============================================

/* TODO: Fix problem 1
 * Requirements:
 * - Declare variables in smallest scope
 * - Use loop-scope declarations (C99+)
 * - Group related calculations
 * - Use block scope where appropriate
 */
void good_data_processing(int *data, size_t size) {
    // TODO: Refactor with minimal scope
    // Hint: Declare variables where they're used
    (void)data;
    (void)size;
}

/* TODO: Fix problem 2
 * Requirements:
 * - Remove global variables
 * - Create sensor state structure
 * - Pass state as parameter
 * - Return values instead of side effects
 */
typedef struct {
    int sensor_id;
    int reading_count;
    double last_value;
    bool calibrated;
} SensorState;

void good_read_sensor(SensorState *state) {
    // TODO: Use state parameter, no globals
    (void)state;
}

/* TODO: Fix problem 3
 * Requirements:
 * - Use different variable names
 * - Declare in appropriate scopes
 * - Make purpose clear from name
 */
void good_minimal_scope(int *array, size_t size) {
    // TODO: Use properly scoped variables
    (void)array;
    (void)size;
}

// ============================================
// ADVANCED CHALLENGE
// ============================================

/* TODO: Refactor this complex function
 * Requirements:
 * - Minimize all variable scopes
 * - Extract helper functions if needed
 * - No variable lives longer than necessary
 * - Clear, descriptive names
 */
void bad_complex_processing(double *samples, size_t count) {
    int i, j;
    double temp;
    double sum, mean, variance, std_dev;
    double min, max;
    int outlier_count;
    double threshold;
    bool is_outlier;
    
    // Calculate mean
    sum = 0.0;
    for (i = 0; i < (int)count; i++) {
        sum += samples[i];
    }
    mean = sum / count;
    
    // Calculate variance
    sum = 0.0;
    for (i = 0; i < (int)count; i++) {
        temp = samples[i] - mean;
        sum += temp * temp;
    }
    variance = sum / count;
    std_dev = sqrt(variance);
    
    // Find outliers (> 2 std dev)
    threshold = 2.0 * std_dev;
    outlier_count = 0;
    for (i = 0; i < (int)count; i++) {
        temp = fabs(samples[i] - mean);
        is_outlier = (temp > threshold);
        if (is_outlier) {
            outlier_count++;
        }
    }
    
    // Find range
    min = samples[0];
    max = samples[0];
    for (i = 1; i < (int)count; i++) {
        if (samples[i] < min) min = samples[i];
        if (samples[i] > max) max = samples[i];
    }
    
    printf("Stats:\n");
    printf("  Mean: %.2f\n", mean);
    printf("  Std Dev: %.2f\n", std_dev);
    printf("  Range: %.2f - %.2f\n", min, max);
    printf("  Outliers: %d\n", outlier_count);
}

typedef struct {
    double mean;
    double std_dev;
    double min;
    double max;
    int outlier_count;
} Statistics;

Statistics good_complex_processing(const double *samples, size_t count) {
    Statistics stats = {0};
    // TODO: Refactor with minimal scope
    // Hint: Use block scope for intermediate calculations
    (void)samples;
    (void)count;
    return stats;
}

// ============================================
// TEST HARNESS
// ============================================

void test_data_processing(void) {
    printf("Test 1: Data Processing\n");
    
    int data[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    size_t size = sizeof(data) / sizeof(data[0]);
    
    printf("  Bad version (wide scope):\n");
    int bad_data[9];
    memcpy(bad_data, data, sizeof(data));
    bad_data_processing(bad_data, size);
    
    printf("  Good version (minimal scope):\n");
    int good_data[9];
    memcpy(good_data, data, sizeof(data));
    good_data_processing(good_data, size);
    printf("\n");
}

void test_sensor_reading(void) {
    printf("Test 2: Sensor Reading\n");
    
    printf("  Bad version (globals):\n");
    g_sensor_id = 1;
    g_reading_count = 0;
    g_calibrated = true;
    bad_read_sensor();
    printf("    Reading count: %d\n", g_reading_count);
    
    printf("  Good version (no globals):\n");
    SensorState state = {
        .sensor_id = 1,
        .reading_count = 0,
        .calibrated = true
    };
    good_read_sensor(&state);
    printf("    Reading count: %d\n", state.reading_count);
    printf("\n");
}

void test_minimal_scope(void) {
    printf("Test 3: Minimal Scope\n");
    
    int array[] = {9, 5, 7, 3, 1};
    size_t size = sizeof(array) / sizeof(array[0]);
    
    printf("  Bad version (reused variables):\n");
    int bad_array[5];
    memcpy(bad_array, array, sizeof(array));
    bad_reused_variables(bad_array, size);
    
    printf("  Good version (proper scope):\n");
    int good_array[5];
    memcpy(good_array, array, sizeof(array));
    good_minimal_scope(good_array, size);
    printf("\n");
}

void test_complex_processing(void) {
    printf("Test 4: Complex Processing\n");
    
    double samples[] = {10.0, 12.0, 11.5, 50.0, 10.8, 11.2, 10.5};
    size_t count = sizeof(samples) / sizeof(samples[0]);
    
    printf("  Bad version:\n");
    bad_complex_processing(samples, count);
    
    printf("  Good version:\n");
    Statistics stats = good_complex_processing(samples, count);
    printf("    Mean: %.2f\n", stats.mean);
    printf("    Std Dev: %.2f\n", stats.std_dev);
    printf("    Range: %.2f - %.2f\n", stats.min, stats.max);
    printf("    Outliers: %d\n", stats.outlier_count);
    printf("\n");
}

int main(void) {
    printf("EXERCISE 6: LIMIT VARIABLE SCOPE\n");
    printf("=================================\n\n");
    
    test_data_processing();
    test_sensor_reading();
    test_minimal_scope();
    test_complex_processing();
    
    printf("✅ Exercise 6 complete!\n");
    printf("\nHints:\n");
    printf("1. Declare loop variables in for() statement\n");
    printf("2. Use { } blocks to limit scope\n");
    printf("3. Declare variables right before use\n");
    printf("4. Pass state as parameters, not globals\n");
    printf("5. Use descriptive names (not 'temp', 'i')\n");
    printf("6. Return values via struct or out-params\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] No global variables (except const)
 * [ ] Variables declared in minimal scope
 * [ ] Loop variables in for() statement
 * [ ] No variable outlives its use
 * [ ] Clear, descriptive names
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Use block scope { } effectively
 * [ ] Extract helper functions
 * [ ] No magic numbers
 * [ ] Document scope decisions
 */
