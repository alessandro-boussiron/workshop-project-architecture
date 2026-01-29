/*
 * EXERCISE 3: STATIC MEMORY ALLOCATION
 * 
 * Task: Convert dynamic allocation to static
 * - Remove all malloc/free
 * - Use object pools
 * - Use fixed-size buffers
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex03_static_memory.c -o ex03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

// TODO: Add your MAX_ defines here
// #define MAX_SENSORS ???
// #define MAX_NAME_LENGTH ???
// #define MAX_DATA_POINTS ???

// ============================================
// ❌ BAD CODE TO FIX
// ============================================

/* Problem: Dynamic sensor allocation */
typedef struct {
    char *name;
    int *data;
    size_t data_size;
    bool active;
} DynamicSensor;

DynamicSensor* bad_create_sensor(const char *name, size_t data_size) {
    DynamicSensor *sensor = malloc(sizeof(DynamicSensor));
    if (sensor == NULL) {
        return NULL;
    }
    
    sensor->name = malloc(strlen(name) + 1);
    if (sensor->name == NULL) {
        free(sensor);
        return NULL;
    }
    strcpy(sensor->name, name);
    
    sensor->data = malloc(data_size * sizeof(int));
    if (sensor->data == NULL) {
        free(sensor->name);
        free(sensor);
        return NULL;
    }
    
    sensor->data_size = data_size;
    sensor->active = true;
    
    return sensor;
}

void bad_destroy_sensor(DynamicSensor *sensor) {
    if (sensor != NULL) {
        free(sensor->data);
        free(sensor->name);
        free(sensor);
    }
}

void bad_add_data_point(DynamicSensor *sensor, int value) {
    if (sensor == NULL) {
        return;
    }
    
    // Grow array (very bad!)
    sensor->data_size++;
    sensor->data = realloc(sensor->data, sensor->data_size * sizeof(int));
    sensor->data[sensor->data_size - 1] = value;
}

// ============================================
// ✅ YOUR TASK: FIX THE CODE BELOW
// ============================================

/* TODO: Define static sensor structure
 * Requirements:
 * - Fixed-size name buffer (MAX_NAME_LENGTH)
 * - Fixed-size data array (MAX_DATA_POINTS)
 * - No pointers to allocated memory
 * - Include 'active' flag for pool
 */
typedef struct {
    // TODO: Define static sensor structure
    char name[64];  // Example - adjust size
    int data[100];  // Example - adjust size
    size_t data_count;
    bool active;
} StaticSensor;

/* TODO: Define sensor pool
 * Requirements:
 * - Array of MAX_SENSORS
 * - Global or static variable
 * - Track allocated count
 */
typedef struct {
    // TODO: Define sensor pool
    StaticSensor sensors[10];  // Example - adjust size
    size_t allocated_count;
} SensorPool;

// TODO: Declare global pool
static SensorPool g_sensor_pool = {0};

/* TODO: Initialize pool
 * Requirements:
 * - Set all sensors inactive
 * - Reset counters
 * - No malloc
 */
void pool_init(void) {
    // TODO: Implement pool initialization
}

/* TODO: Acquire sensor from pool
 * Requirements:
 * - Find inactive sensor
 * - Mark as active
 * - Copy name (bounded)
 * - Initialize data
 * - Return pointer or NULL if full
 */
StaticSensor* good_create_sensor(const char *name) {
    // TODO: Implement sensor acquisition
    (void)name;
    return NULL;  // Placeholder
}

/* TODO: Release sensor to pool
 * Requirements:
 * - Verify sensor from pool
 * - Clear sensor data
 * - Mark inactive
 * - Decrement counter
 */
void good_destroy_sensor(StaticSensor *sensor) {
    // TODO: Implement sensor release
    (void)sensor;
}

/* TODO: Add data point
 * Requirements:
 * - Check if space available
 * - Add to fixed array
 * - Increment counter
 * - Return success/failure
 */
bool good_add_data_point(StaticSensor *sensor, int value) {
    // TODO: Implement data addition
    (void)sensor;
    (void)value;
    return false;  // Placeholder
}

/* TODO: Get sensor statistics
 * Requirements:
 * - Calculate average
 * - Find min/max
 * - Return count
 */
void good_get_stats(const StaticSensor *sensor, 
                    double *avg, int *min, int *max) {
    // TODO: Implement statistics
    (void)sensor;
    (void)avg;
    (void)min;
    (void)max;
}

// ============================================
// TEST HARNESS
// ============================================

void test_sensor_lifecycle(void) {
    printf("Test 1: Sensor Lifecycle\n");
    
    // Bad version
    printf("  Bad version (dynamic):\n");
    DynamicSensor *bad_sensor = bad_create_sensor("Temp01", 10);
    if (bad_sensor) {
        printf("    Created sensor: %s\n", bad_sensor->name);
        bad_destroy_sensor(bad_sensor);
        printf("    Destroyed sensor\n");
    }
    
    // Good version
    printf("  Good version (static):\n");
    pool_init();
    StaticSensor *good_sensor = good_create_sensor("Temp01");
    if (good_sensor) {
        printf("    Acquired sensor: %s\n", good_sensor->name);
        good_destroy_sensor(good_sensor);
        printf("    Released sensor\n");
    }
    printf("\n");
}

void test_data_operations(void) {
    printf("Test 2: Data Operations\n");
    
    pool_init();
    StaticSensor *sensor = good_create_sensor("Pressure");
    if (sensor == NULL) {
        printf("  Failed to acquire sensor\n");
        return;
    }
    
    printf("  Adding data points...\n");
    for (int i = 0; i < 10; i++) {
        bool success = good_add_data_point(sensor, 100 + i);
        if (!success) {
            printf("    Failed at point %d\n", i);
            break;
        }
    }
    
    double avg;
    int min, max;
    good_get_stats(sensor, &avg, &min, &max);
    printf("  Stats - Avg: %.1f, Min: %d, Max: %d\n", avg, min, max);
    
    good_destroy_sensor(sensor);
    printf("\n");
}

void test_pool_exhaustion(void) {
    printf("Test 3: Pool Exhaustion\n");
    
    pool_init();
    
    printf("  Acquiring sensors until pool exhausted...\n");
    int count = 0;
    for (int i = 0; i < 20; i++) {  // Try more than pool size
        char name[32];
        snprintf(name, sizeof(name), "Sensor%d", i);
        
        StaticSensor *sensor = good_create_sensor(name);
        if (sensor != NULL) {
            count++;
        } else {
            printf("  Pool exhausted after %d sensors\n", count);
            break;
        }
    }
    printf("\n");
}

int main(void) {
    printf("EXERCISE 3: STATIC MEMORY ALLOCATION\n");
    printf("=====================================\n\n");
    
    test_sensor_lifecycle();
    test_data_operations();
    test_pool_exhaustion();
    
    printf("✅ Exercise 3 complete!\n");
    printf("\nHints:\n");
    printf("1. Use char arrays, not char*\n");
    printf("2. Use int arrays, not int*\n");
    printf("3. Track count/size explicitly\n");
    printf("4. Implement acquire/release pattern\n");
    printf("5. Check bounds before adding data\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] No malloc/free/realloc
 * [ ] All buffers fixed-size
 * [ ] Pool-based allocation
 * [ ] Proper bounds checking
 * [ ] Pool exhaustion handled
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Add pool statistics (peak usage)
 * [ ] Implement pool reset function
 * [ ] Add sensor search by name
 * [ ] Thread-safety considerations
 */
