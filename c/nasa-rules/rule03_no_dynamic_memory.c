/*
 * NASA RULE 3: NO DYNAMIC MEMORY AFTER INIT
 * 
 * No malloc/free after initialization
 * Use static allocation or pre-allocated pools
 * 
 * Compilation: gcc -Wall -Wextra -Werror -std=c11 rule03_no_dynamic_memory.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_OBJECTS 32
#define MAX_BUFFER_SIZE 256
#define MAX_EVENTS 64

// ============================================
// ❌ BAD EXAMPLES - Runtime allocation
// ============================================

/* BAD: Allocating during runtime */
typedef struct {
    int *data;
    size_t size;
} BadDynamicArray;

BadDynamicArray* bad_create_array(size_t size) {
    BadDynamicArray *array = malloc(sizeof(BadDynamicArray));  // Runtime malloc
    if (array == NULL) {
        return NULL;
    }
    
    array->data = malloc(size * sizeof(int));  // More runtime malloc
    if (array->data == NULL) {
        free(array);
        return NULL;
    }
    
    array->size = size;
    return array;
}

void bad_destroy_array(BadDynamicArray *array) {
    if (array != NULL) {
        free(array->data);
        free(array);
    }
}

/* BAD: Allocating in processing loop */
void bad_process_messages(int message_count) {
    for (int i = 0; i < message_count; i++) {
        char *buffer = malloc(128);  // Allocation in loop!
        
        if (buffer != NULL) {
            snprintf(buffer, 128, "Message %d", i);
            printf("%s\n", buffer);
            free(buffer);
        }
    }
}

/* BAD: Growing data structures */
typedef struct BadNode {
    int value;
    struct BadNode *next;
} BadNode;

BadNode* bad_list_add(BadNode *head, int value) {
    BadNode *new_node = malloc(sizeof(BadNode));  // Runtime allocation
    if (new_node == NULL) {
        return head;
    }
    
    new_node->value = value;
    new_node->next = head;
    return new_node;
}

// ============================================
// ✅ GOOD EXAMPLES - Static allocation
// ============================================

/* GOOD: Fixed-size array (stack allocation) */
typedef struct {
    int data[MAX_BUFFER_SIZE];
    size_t count;
} StaticArray;

void static_array_init(StaticArray *array) {
    assert(array != NULL);
    array->count = 0;
    memset(array->data, 0, sizeof(array->data));
}

bool static_array_add(StaticArray *array, int value) {
    if (array->count >= MAX_BUFFER_SIZE) {
        return false;  // Array full
    }
    
    array->data[array->count] = value;
    array->count++;
    return true;
}

/* GOOD: Pre-allocated object pool */
typedef struct {
    int id;
    bool active;
    char data[64];
} PoolObject;

typedef struct {
    PoolObject objects[MAX_OBJECTS];
    size_t allocated_count;
} ObjectPool;

static ObjectPool g_object_pool = {0};  // Global, initialized at startup

void pool_init(void) {
    memset(&g_object_pool, 0, sizeof(g_object_pool));
    
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        g_object_pool.objects[i].active = false;
        g_object_pool.objects[i].id = (int)i;
    }
}

PoolObject* pool_acquire(void) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        if (!g_object_pool.objects[i].active) {
            g_object_pool.objects[i].active = true;
            g_object_pool.allocated_count++;
            return &g_object_pool.objects[i];
        }
    }
    
    printf("WARNING: Object pool exhausted\n");
    return NULL;
}

void pool_release(PoolObject *obj) {
    if (obj == NULL) {
        return;
    }
    
    // Verify object is from pool
    if (obj < g_object_pool.objects || 
        obj >= g_object_pool.objects + MAX_OBJECTS) {
        printf("ERROR: Object not from pool\n");
        return;
    }
    
    if (!obj->active) {
        printf("WARNING: Double free detected\n");
        return;
    }
    
    memset(obj->data, 0, sizeof(obj->data));
    obj->active = false;
    g_object_pool.allocated_count--;
}

/* GOOD: Static message buffers */
#define MAX_MESSAGES 16

typedef struct {
    char text[128];
    bool in_use;
} MessageBuffer;

static MessageBuffer g_message_buffers[MAX_MESSAGES] = {0};

MessageBuffer* message_acquire(void) {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (!g_message_buffers[i].in_use) {
            g_message_buffers[i].in_use = true;
            return &g_message_buffers[i];
        }
    }
    return NULL;
}

void message_release(MessageBuffer *msg) {
    if (msg != NULL) {
        memset(msg->text, 0, sizeof(msg->text));
        msg->in_use = false;
    }
}

void good_process_messages(int message_count) {
    // Bounded by pool size
    const int max = (message_count > MAX_MESSAGES) ? MAX_MESSAGES : message_count;
    
    for (int i = 0; i < max; i++) {
        MessageBuffer *msg = message_acquire();
        if (msg != NULL) {
            snprintf(msg->text, sizeof(msg->text), "Message %d", i);
            printf("%s\n", msg->text);
            message_release(msg);
        }
    }
}

/* GOOD: Fixed-size linked list using array */
typedef struct {
    int value;
    int next;  // Index, not pointer
    bool in_use;
} StaticNode;

#define MAX_NODES 100

typedef struct {
    StaticNode nodes[MAX_NODES];
    int head;
    size_t count;
} StaticList;

static StaticList g_list = {.head = -1, .count = 0};

void list_init(void) {
    g_list.head = -1;
    g_list.count = 0;
    
    for (int i = 0; i < MAX_NODES; i++) {
        g_list.nodes[i].in_use = false;
        g_list.nodes[i].next = -1;
    }
}

bool list_add(int value) {
    if (g_list.count >= MAX_NODES) {
        return false;
    }
    
    // Find free node
    int free_idx = -1;
    for (int i = 0; i < MAX_NODES; i++) {
        if (!g_list.nodes[i].in_use) {
            free_idx = i;
            break;
        }
    }
    
    if (free_idx == -1) {
        return false;
    }
    
    // Use the node
    g_list.nodes[free_idx].value = value;
    g_list.nodes[free_idx].in_use = true;
    g_list.nodes[free_idx].next = g_list.head;
    g_list.head = free_idx;
    g_list.count++;
    
    return true;
}

void list_print(void) {
    printf("List: ");
    int current = g_list.head;
    int iterations = 0;
    
    while (current != -1 && iterations < MAX_NODES) {
        printf("%d ", g_list.nodes[current].value);
        current = g_list.nodes[current].next;
        iterations++;
    }
    printf("\n");
}

// ============================================
// REAL-WORLD EXAMPLES
// ============================================

/* Example 1: Event Queue (static) */
typedef struct {
    uint8_t type;
    uint16_t data;
    uint32_t timestamp;
} Event;

typedef struct {
    Event events[MAX_EVENTS];
    size_t head;
    size_t tail;
    size_t count;
} EventQueue;

static EventQueue g_event_queue = {0};

void event_queue_init(void) {
    memset(&g_event_queue, 0, sizeof(g_event_queue));
}

bool event_queue_push(uint8_t type, uint16_t data, uint32_t timestamp) {
    if (g_event_queue.count >= MAX_EVENTS) {
        return false;  // Queue full
    }
    
    Event *event = &g_event_queue.events[g_event_queue.head];
    event->type = type;
    event->data = data;
    event->timestamp = timestamp;
    
    g_event_queue.head = (g_event_queue.head + 1) % MAX_EVENTS;
    g_event_queue.count++;
    
    return true;
}

bool event_queue_pop(Event *out_event) {
    if (g_event_queue.count == 0) {
        return false;  // Queue empty
    }
    
    *out_event = g_event_queue.events[g_event_queue.tail];
    g_event_queue.tail = (g_event_queue.tail + 1) % MAX_EVENTS;
    g_event_queue.count--;
    
    return true;
}

/* Example 2: Fixed-size hash table */
#define HASH_TABLE_SIZE 32

typedef struct {
    int key;
    int value;
    bool occupied;
} HashEntry;

typedef struct {
    HashEntry entries[HASH_TABLE_SIZE];
} HashTable;

static HashTable g_hash_table = {0};

void hash_table_init(void) {
    memset(&g_hash_table, 0, sizeof(g_hash_table));
}

static size_t hash_function(int key) {
    return (size_t)key % HASH_TABLE_SIZE;
}

bool hash_table_insert(int key, int value) {
    size_t index = hash_function(key);
    size_t attempts = 0;
    
    // Linear probing with fixed bound
    while (attempts < HASH_TABLE_SIZE) {
        if (!g_hash_table.entries[index].occupied) {
            g_hash_table.entries[index].key = key;
            g_hash_table.entries[index].value = value;
            g_hash_table.entries[index].occupied = true;
            return true;
        }
        
        index = (index + 1) % HASH_TABLE_SIZE;
        attempts++;
    }
    
    return false;  // Table full
}

bool hash_table_lookup(int key, int *out_value) {
    size_t index = hash_function(key);
    size_t attempts = 0;
    
    while (attempts < HASH_TABLE_SIZE) {
        if (!g_hash_table.entries[index].occupied) {
            return false;  // Not found
        }
        
        if (g_hash_table.entries[index].key == key) {
            *out_value = g_hash_table.entries[index].value;
            return true;
        }
        
        index = (index + 1) % HASH_TABLE_SIZE;
        attempts++;
    }
    
    return false;
}

/* Example 3: Telemetry buffer */
#define MAX_TELEMETRY_SAMPLES 128

typedef struct {
    float temperature;
    float pressure;
    float voltage;
    uint32_t timestamp;
} TelemetrySample;

typedef struct {
    TelemetrySample samples[MAX_TELEMETRY_SAMPLES];
    size_t write_index;
    size_t count;
} TelemetryBuffer;

static TelemetryBuffer g_telemetry = {0};

void telemetry_init(void) {
    memset(&g_telemetry, 0, sizeof(g_telemetry));
}

void telemetry_add_sample(float temp, float pressure, float voltage, uint32_t timestamp) {
    TelemetrySample *sample = &g_telemetry.samples[g_telemetry.write_index];
    
    sample->temperature = temp;
    sample->pressure = pressure;
    sample->voltage = voltage;
    sample->timestamp = timestamp;
    
    g_telemetry.write_index = (g_telemetry.write_index + 1) % MAX_TELEMETRY_SAMPLES;
    
    if (g_telemetry.count < MAX_TELEMETRY_SAMPLES) {
        g_telemetry.count++;
    }
}

void telemetry_get_stats(float *avg_temp, float *avg_pressure) {
    if (g_telemetry.count == 0) {
        *avg_temp = 0.0f;
        *avg_pressure = 0.0f;
        return;
    }
    
    float temp_sum = 0.0f;
    float pressure_sum = 0.0f;
    
    for (size_t i = 0; i < g_telemetry.count; i++) {
        temp_sum += g_telemetry.samples[i].temperature;
        pressure_sum += g_telemetry.samples[i].pressure;
    }
    
    *avg_temp = temp_sum / g_telemetry.count;
    *avg_pressure = pressure_sum / g_telemetry.count;
}

// ============================================
// MAIN - Demonstrations
// ============================================

int main(void) {
    printf("NASA RULE 3: NO DYNAMIC MEMORY AFTER INIT\n");
    printf("==========================================\n\n");
    
    // Test 1: Object pool
    printf("Test 1: Object Pool\n");
    pool_init();
    
    PoolObject *obj1 = pool_acquire();
    PoolObject *obj2 = pool_acquire();
    
    if (obj1 && obj2) {
        snprintf(obj1->data, sizeof(obj1->data), "Object 1");
        snprintf(obj2->data, sizeof(obj2->data), "Object 2");
        printf("  Acquired: %s, %s\n", obj1->data, obj2->data);
        printf("  Pool usage: %zu/%d\n", g_object_pool.allocated_count, MAX_OBJECTS);
        
        pool_release(obj1);
        pool_release(obj2);
        printf("  Released. Pool usage: %zu/%d\n\n", g_object_pool.allocated_count, MAX_OBJECTS);
    }
    
    // Test 2: Static list
    printf("Test 2: Static Linked List\n");
    list_init();
    list_add(10);
    list_add(20);
    list_add(30);
    printf("  ");
    list_print();
    printf("\n");
    
    // Test 3: Event queue
    printf("Test 3: Event Queue\n");
    event_queue_init();
    
    event_queue_push(1, 100, 1000);
    event_queue_push(2, 200, 2000);
    event_queue_push(3, 300, 3000);
    
    Event event;
    while (event_queue_pop(&event)) {
        printf("  Event type=%d, data=%d, time=%u\n", 
               event.type, event.data, event.timestamp);
    }
    printf("\n");
    
    // Test 4: Hash table
    printf("Test 4: Hash Table\n");
    hash_table_init();
    
    hash_table_insert(42, 1000);
    hash_table_insert(17, 2000);
    hash_table_insert(99, 3000);
    
    int value;
    if (hash_table_lookup(42, &value)) {
        printf("  Key 42 -> Value %d\n", value);
    }
    if (hash_table_lookup(17, &value)) {
        printf("  Key 17 -> Value %d\n", value);
    }
    printf("\n");
    
    // Test 5: Telemetry
    printf("Test 5: Telemetry Buffer\n");
    telemetry_init();
    
    telemetry_add_sample(25.5f, 101.3f, 3.3f, 1000);
    telemetry_add_sample(26.0f, 101.2f, 3.4f, 2000);
    telemetry_add_sample(25.8f, 101.4f, 3.3f, 3000);
    
    float avg_temp, avg_pressure;
    telemetry_get_stats(&avg_temp, &avg_pressure);
    printf("  Average temp: %.1f°C, pressure: %.1fkPa\n\n", avg_temp, avg_pressure);
    
    printf("✅ Rule 3 Examples Complete\n");
    printf("\n📊 Memory Usage Summary:\n");
    printf("  All memory allocated statically at compile time\n");
    printf("  No malloc/free called during runtime\n");
    printf("  Predictable memory footprint\n");
    
    return 0;
}

/*
 * KEY TAKEAWAYS - RULE 3
 * ======================
 *
 * ✅ DO:
 * - Use static/global arrays
 * - Pre-allocate object pools
 * - Use fixed-size buffers
 * - Allocate on stack when possible
 * - Reuse memory via pools
 *
 * ❌ DON'T:
 * - Call malloc/free in runtime
 * - Grow data structures dynamically
 * - Use variable-length arrays (VLA)
 * - Allocate in loops
 * - Depend on heap memory
 *
 * WHY THIS MATTERS:
 * - No memory fragmentation
 * - No allocation failures during operation
 * - Predictable memory usage
 * - No heap exhaustion
 * - Deterministic timing
 * - Simpler memory analysis
 *
 * PATTERNS:
 * 1. Object Pool - Pre-allocated objects
 * 2. Ring Buffer - Fixed-size circular buffer
 * 3. Static Lists - Array-based linked structures
 * 4. Arena Allocator - Single allocation block
 *
 * INITIALIZATION PHASE:
 * - Can use malloc during startup
 * - Once running, only reuse existing memory
 * - Clear separation of init vs runtime
 */
