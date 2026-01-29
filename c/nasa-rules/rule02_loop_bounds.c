/*
 * NASA RULE 2: FIXED LOOP BOUNDS
 * 
 * All loops must have a fixed upper bound
 * Must be able to prove loop termination
 * 
 * Compilation: gcc -Wall -Wextra -Werror -std=c11 rule02_loop_bounds.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256
#define MAX_ITERATIONS 1000
#define MAX_ARRAY_SIZE 100

// ============================================
// ❌ BAD EXAMPLES - Unbounded loops
// ============================================

/* BAD: Infinite loop with break inside */
void bad_infinite_loop(void) {
    int counter = 0;
    while (1) {  // No fixed bound visible
        counter++;
        if (counter > 1000) {
            break;  // Exit hidden in loop body
        }
        printf("Count: %d\n", counter);
    }
}

/* BAD: Loop bound depends on external state */
int bad_global_counter = 0;

void bad_external_bound(void) {
    while (bad_global_counter < 100) {  // Bound depends on global
        printf("%d ", bad_global_counter);
        bad_global_counter++;
    }
}

/* BAD: Loop with no guaranteed termination */
void bad_search_unbounded(int *array, int target) {
    int i = 0;
    while (array[i] != target) {  // What if target not found?
        i++;
        printf("Searching at %d\n", i);
    }
}

/* BAD: Do-while with unclear bound */
void bad_do_while(void) {
    int value;
    do {
        value = rand() % 100;  // Unpredictable
        printf("Value: %d\n", value);
    } while (value != 42);  // May never terminate
}

/* BAD: Nested loops without clear bounds */
void bad_nested_loops(int n) {
    for (int i = 0; i < n; i++) {
        int j = 0;
        while (j < i * 2) {  // Bound changes with i
            printf("%d,%d ", i, j);
            j++;
        }
    }
}

// ============================================
// ✅ GOOD EXAMPLES - Fixed loop bounds
// ============================================

/* GOOD: Loop with constant bound */
void good_fixed_loop(void) {
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        printf("Iteration %d\n", i);
    }
}

/* GOOD: Array traversal with size parameter */
void good_array_loop(const int *array, size_t size) {
    // Ensure size doesn't exceed maximum
    const size_t safe_size = (size > MAX_ARRAY_SIZE) ? MAX_ARRAY_SIZE : size;
    
    for (size_t i = 0; i < safe_size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

/* GOOD: Search with maximum iterations */
int good_search_bounded(const int *array, size_t size, int target) {
    const size_t max_search = (size > MAX_ARRAY_SIZE) ? MAX_ARRAY_SIZE : size;
    
    for (size_t i = 0; i < max_search; i++) {
        if (array[i] == target) {
            return (int)i;  // Found
        }
    }
    
    return -1;  // Not found within bounds
}

/* GOOD: String processing with maximum length */
size_t good_string_length(const char *str) {
    if (str == NULL) {
        return 0;
    }
    
    size_t len = 0;
    // Guaranteed termination within MAX_BUFFER_SIZE
    while (len < MAX_BUFFER_SIZE && str[len] != '\0') {
        len++;
    }
    
    return len;
}

/* GOOD: Nested loops with fixed bounds */
void good_nested_loops(void) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("(%d,%d) ", i, j);
        }
        printf("\n");
    }
}

/* GOOD: While loop with guaranteed bound */
#define MAX_RETRIES 5

bool good_retry_with_limit(void) {
    int attempts = 0;
    bool success = false;
    
    while (attempts < MAX_RETRIES && !success) {
        printf("Attempt %d\n", attempts);
        
        // Simulate operation
        success = (rand() % 3 == 0);
        attempts++;
    }
    
    return success;
}

// ============================================
// REAL-WORLD EXAMPLES
// ============================================

/* Example 1: Ring buffer with fixed iterations */
typedef struct {
    uint8_t data[MAX_BUFFER_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} RingBuffer;

void ring_buffer_init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool ring_buffer_write(RingBuffer *rb, uint8_t byte) {
    if (rb->count >= MAX_BUFFER_SIZE) {
        return false;  // Buffer full
    }
    
    rb->data[rb->head] = byte;
    rb->head = (rb->head + 1) % MAX_BUFFER_SIZE;  // Wrap around
    rb->count++;
    
    return true;
}

bool ring_buffer_read(RingBuffer *rb, uint8_t *byte) {
    if (rb->count == 0) {
        return false;  // Buffer empty
    }
    
    *byte = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) % MAX_BUFFER_SIZE;  // Wrap around
    rb->count--;
    
    return true;
}

void ring_buffer_process_all(RingBuffer *rb) {
    // Process with guaranteed bound
    size_t items_to_process = rb->count;
    if (items_to_process > MAX_BUFFER_SIZE) {
        items_to_process = MAX_BUFFER_SIZE;
    }
    
    for (size_t i = 0; i < items_to_process; i++) {
        uint8_t byte;
        if (ring_buffer_read(rb, &byte)) {
            printf("Processed: %d\n", byte);
        }
    }
}

/* Example 2: Data filtering with bounds */
typedef struct {
    int values[MAX_ARRAY_SIZE];
    size_t count;
} DataSet;

void dataset_filter_outliers(DataSet *input, DataSet *output, int threshold) {
    output->count = 0;
    
    // Fixed bound based on input size and maximum
    const size_t max_items = (input->count > MAX_ARRAY_SIZE) ? 
                             MAX_ARRAY_SIZE : input->count;
    
    for (size_t i = 0; i < max_items; i++) {
        if (abs(input->values[i]) <= threshold) {
            if (output->count < MAX_ARRAY_SIZE) {
                output->values[output->count] = input->values[i];
                output->count++;
            }
        }
    }
}

/* Example 3: Protocol parser with timeout */
#define MAX_PACKET_SIZE 128
#define MAX_PARSE_ATTEMPTS 10

typedef enum {
    PARSE_SUCCESS,
    PARSE_INCOMPLETE,
    PARSE_ERROR
} ParseResult;

typedef struct {
    uint8_t buffer[MAX_PACKET_SIZE];
    size_t bytes_received;
    bool complete;
} PacketParser;

ParseResult parse_packet_bounded(PacketParser *parser, uint8_t byte) {
    // Bounds check
    if (parser->bytes_received >= MAX_PACKET_SIZE) {
        return PARSE_ERROR;
    }
    
    parser->buffer[parser->bytes_received] = byte;
    parser->bytes_received++;
    
    // Check for end marker
    if (byte == 0xFF) {
        parser->complete = true;
        return PARSE_SUCCESS;
    }
    
    return PARSE_INCOMPLETE;
}

void process_stream_bounded(const uint8_t *stream, size_t stream_size) {
    PacketParser parser = {0};
    
    // Guarantee termination with fixed bound
    const size_t max_bytes = (stream_size > MAX_PACKET_SIZE) ? 
                             MAX_PACKET_SIZE : stream_size;
    
    for (size_t i = 0; i < max_bytes; i++) {
        ParseResult result = parse_packet_bounded(&parser, stream[i]);
        
        if (result == PARSE_SUCCESS) {
            printf("Packet complete at byte %zu\n", i);
            break;
        } else if (result == PARSE_ERROR) {
            printf("Parse error at byte %zu\n", i);
            break;
        }
    }
}

/* Example 4: Moving average filter */
#define FILTER_SIZE 8

typedef struct {
    int samples[FILTER_SIZE];
    size_t index;
    size_t count;
} MovingAverageFilter;

void filter_init(MovingAverageFilter *filter) {
    memset(filter->samples, 0, sizeof(filter->samples));
    filter->index = 0;
    filter->count = 0;
}

void filter_add_sample(MovingAverageFilter *filter, int sample) {
    filter->samples[filter->index] = sample;
    filter->index = (filter->index + 1) % FILTER_SIZE;
    
    if (filter->count < FILTER_SIZE) {
        filter->count++;
    }
}

int filter_get_average(const MovingAverageFilter *filter) {
    if (filter->count == 0) {
        return 0;
    }
    
    int sum = 0;
    // Fixed bound - always FILTER_SIZE
    for (size_t i = 0; i < FILTER_SIZE; i++) {
        sum += filter->samples[i];
    }
    
    return sum / FILTER_SIZE;
}

/* Example 5: Matrix operations */
#define MATRIX_SIZE 10

typedef struct {
    int data[MATRIX_SIZE][MATRIX_SIZE];
} Matrix;

void matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result) {
    // Triple nested loop with fixed bounds
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            int sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += a->data[i][k] * b->data[k][j];
            }
            result->data[i][j] = sum;
        }
    }
}

// ============================================
// MAIN - Demonstrations
// ============================================

int main(void) {
    printf("NASA RULE 2: FIXED LOOP BOUNDS\n");
    printf("===============================\n\n");
    
    // Test 1: Array processing
    printf("Test 1: Array Processing\n");
    int array[] = {10, 20, 30, 40, 50};
    good_array_loop(array, 5);
    printf("\n");
    
    // Test 2: Search with bounds
    printf("Test 2: Bounded Search\n");
    int pos = good_search_bounded(array, 5, 30);
    printf("Found 30 at position: %d\n\n", pos);
    
    // Test 3: String length
    printf("Test 3: String Length (bounded)\n");
    const char *str = "Hello, NASA!";
    size_t len = good_string_length(str);
    printf("String length: %zu\n\n", len);
    
    // Test 4: Ring buffer
    printf("Test 4: Ring Buffer\n");
    RingBuffer rb;
    ring_buffer_init(&rb);
    
    for (int i = 0; i < 5; i++) {
        ring_buffer_write(&rb, (uint8_t)(i + 10));
    }
    
    ring_buffer_process_all(&rb);
    printf("\n");
    
    // Test 5: Moving average filter
    printf("Test 5: Moving Average Filter\n");
    MovingAverageFilter filter;
    filter_init(&filter);
    
    int samples[] = {100, 102, 98, 101, 99, 103, 97, 100};
    for (size_t i = 0; i < 8; i++) {
        filter_add_sample(&filter, samples[i]);
    }
    
    printf("Average: %d\n\n", filter_get_average(&filter));
    
    // Test 6: Retry mechanism
    printf("Test 6: Retry with Limit\n");
    bool success = good_retry_with_limit();
    printf("Operation %s\n\n", success ? "succeeded" : "failed");
    
    printf("✅ Rule 2 Examples Complete\n");
    return 0;
}

/*
 * KEY TAKEAWAYS - RULE 2
 * ======================
 *
 * ✅ DO:
 * - Use for loops with constant upper bounds
 * - Define maximum iterations as constants
 * - Bound all while loops explicitly
 * - Check array sizes against maximums
 * - Use size parameters with maximum limits
 *
 * ❌ DON'T:
 * - Use while(1) without clear bound
 * - Depend on external state for termination
 * - Use unbounded search/scan operations
 * - Trust that sentinel values exist
 * - Allow dynamic loop bounds without limits
 *
 * WHY THIS MATTERS:
 * - Guaranteed termination
 * - Predictable execution time
 * - Provable correctness
 * - No infinite loops in production
 * - Static analysis can verify bounds
 * - Critical for real-time systems
 *
 * COMPLEXITY ANALYSIS:
 * - Single loop: O(n) where n <= MAX_BOUND
 * - Nested loops: O(n*m) where both bounded
 * - Always knowable worst-case time
 */
