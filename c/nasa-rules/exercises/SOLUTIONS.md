# NASA Rules Exercises - Solutions Guide

Ce document contient les solutions des exercices sur les règles NASA (Rules 1-10).

> **Note**: Ces solutions sont des exemples. Il existe plusieurs façons correctes de résoudre chaque exercice.

## Table des matières

1. [Exercise 1: Control Flow](#exercise-1-control-flow)
2. [Exercise 2: Loop Bounds](#exercise-2-loop-bounds)
3. [Exercise 3: Static Memory](#exercise-3-static-memory)
4. [Exercise 4: Function Size](#exercise-4-function-size)
5. [Exercise 5: Check Returns](#exercise-5-check-returns)
6. [Exercise 6: Limit Scope](#exercise-6-limit-scope)
7. [Exercise 7: Assertions](#exercise-7-assertions)
8. [Exercise 8: Pointer Indirection](#exercise-8-pointer-indirection)
9. [Exercise 9: Zero Warnings](#exercise-9-zero-warnings)
10. [Exercise 10: Static Analysis](#exercise-10-static-analysis)

---

## Exercise 1: Control Flow

### Solution - process_commands

```c
typedef enum {
    CMD_START,
    CMD_STOP,
    CMD_PAUSE,
    CMD_UNKNOWN
} CommandType;

CommandType parse_command(const char *cmd) {
    if (strcmp(cmd, "START") == 0) return CMD_START;
    if (strcmp(cmd, "STOP") == 0) return CMD_STOP;
    if (strcmp(cmd, "PAUSE") == 0) return CMD_PAUSE;
    return CMD_UNKNOWN;
}

int good_process_commands(char **commands, int count) {
    if (commands == NULL || count < 0) {
        return -1;k
    }
    
    const int safe_count = (count > MAX_COMMANDS) ? MAX_COMMANDS : count;
    
    for (int i = 0; i < safe_count; i++) {
        CommandType cmd = parse_command(commands[i]);
        
        switch (cmd) {
            case CMD_START:
                printf("Starting...\n");
                break;
                
            case CMD_STOP:
                printf("Stopping...\n");
                return i + 1;  // Stop processing
                
            case CMD_PAUSE:
                printf("Pausing...\n");
                break;
                
            case CMD_UNKNOWN:
                printf("Unknown command: %s\n", commands[i]);
                break;
        }
    }
    
    return safe_count;
}
```

### Solution - initialize_system

```c
int good_initialize_system(const char *config_file) {
    if (config_file == NULL) {
        return -1;
    }
    
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        return -1;
    }
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return -1;
    }
    
    // More initialization...
    
    fclose(file);
    return 0;
}
```

### Solution - iterative_factorial

```c
#define MAX_FACTORIAL_INPUT 20

int good_iterative_factorial(int n) {
    if (n < 0 || n > MAX_FACTORIAL_INPUT) {
        return -1;
    }
    
    if (n <= 1) {
        return 1;
    }
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    
    return result;
}
```

---

## Exercise 2: Loop Bounds

### Constants

```c
#define MAX_INPUT_SIZE 256
#define MAX_ARRAY_SIZE 100
#define MAX_WAIT_CYCLES 1000
#define MAX_STREAM_SIZE 512
#define MAX_OUTER 20
#define MAX_INNER 40
```

### Solution - parse_input

```c
size_t good_parse_input(char *input) {
    if (input == NULL) {
        return 0;
    }
    
    size_t len = 0;
    while (len < MAX_INPUT_SIZE && input[len] != '\0') {
        printf("%c", input[len]);
        len++;
    }
    printf("\n");
    
    return len;
}
```

### Solution - find_value

```c
int good_find_value(int *array, size_t size, int value) {
    if (array == NULL) {
        return -1;
    }
    
    const size_t safe_size = (size > MAX_ARRAY_SIZE) ? MAX_ARRAY_SIZE : size;
    
    for (size_t i = 0; i < safe_size; i++) {
        if (array[i] == value) {
            return (int)i;
        }
    }
    
    return -1;  // Not found
}
```

### Solution - wait_for_ready

```c
bool good_wait_for_ready(volatile bool *ready) {
    if (ready == NULL) {
        return false;
    }
    
    for (int i = 0; i < MAX_WAIT_CYCLES; i++) {
        if (*ready) {
            return true;
        }
        // Small delay (platform specific)
    }
    
    return false;  // Timeout
}
```

### Solution - process_stream

```c
int good_process_stream(uint8_t *stream, size_t max_size) {
    if (stream == NULL || max_size == 0) {
        return 0;
    }
    
    const size_t safe_size = (max_size > MAX_STREAM_SIZE) ? 
                             MAX_STREAM_SIZE : max_size;
    
    int count = 0;
    for (size_t i = 0; i < safe_size; i++) {
        if (stream[i] == 0xFF) {
            break;  // Found sentinel
        }
        printf("Byte: 0x%02X\n", stream[i]);
        count++;
    }
    
    return count;
}
```

### Solution - nested_loops

```c
void good_nested_loops(int n) {
    const int safe_n = (n > MAX_OUTER) ? MAX_OUTER : n;
    
    for (int i = 0; i < safe_n; i++) {
        const int inner_bound = (i * 2 > MAX_INNER) ? MAX_INNER : i * 2;
        
        for (int j = 0; j < inner_bound; j++) {
            printf("%d,%d ", i, j);
        }
        printf("\n");
    }
}
```

---

## Exercise 3: Static Memory

### Constants and Structures

```c
#define MAX_SENSORS 10
#define MAX_NAME_LENGTH 32
#define MAX_DATA_POINTS 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int data[MAX_DATA_POINTS];
    size_t data_count;
    int id;
    bool active;
} StaticSensor;

typedef struct {
    StaticSensor sensors[MAX_SENSORS];
    size_t allocated_count;
} SensorPool;

static SensorPool g_sensor_pool = {0};
```

### Solution - pool_init

```c
void pool_init(void) {
    memset(&g_sensor_pool, 0, sizeof(g_sensor_pool));
    
    for (size_t i = 0; i < MAX_SENSORS; i++) {
        g_sensor_pool.sensors[i].active = false;
        g_sensor_pool.sensors[i].id = (int)i;
        g_sensor_pool.sensors[i].data_count = 0;
    }
    
    g_sensor_pool.allocated_count = 0;
}
```

### Solution - create_sensor

```c
StaticSensor* good_create_sensor(const char *name) {
    if (name == NULL) {
        return NULL;
    }
    
    // Find free sensor
    for (size_t i = 0; i < MAX_SENSORS; i++) {
        if (!g_sensor_pool.sensors[i].active) {
            StaticSensor *sensor = &g_sensor_pool.sensors[i];
            
            // Copy name with bounds check
            strncpy(sensor->name, name, MAX_NAME_LENGTH - 1);
            sensor->name[MAX_NAME_LENGTH - 1] = '\0';
            
            sensor->data_count = 0;
            sensor->active = true;
            g_sensor_pool.allocated_count++;
            
            return sensor;
        }
    }
    
    return NULL;  // Pool full
}
```

### Solution - destroy_sensor

```c
void good_destroy_sensor(StaticSensor *sensor) {
    if (sensor == NULL) {
        return;
    }
    
    // Verify sensor is from pool
    if (sensor < g_sensor_pool.sensors || 
        sensor >= g_sensor_pool.sensors + MAX_SENSORS) {
        return;
    }
    
    if (!sensor->active) {
        return;  // Already released
    }
    
    // Clear sensor
    memset(sensor->name, 0, MAX_NAME_LENGTH);
    memset(sensor->data, 0, sizeof(sensor->data));
    sensor->data_count = 0;
    sensor->active = false;
    
    g_sensor_pool.allocated_count--;
}
```

### Solution - add_data_point

```c
bool good_add_data_point(StaticSensor *sensor, int value) {
    if (sensor == NULL || !sensor->active) {
        return false;
    }
    
    if (sensor->data_count >= MAX_DATA_POINTS) {
        return false;  // Buffer full
    }
    
    sensor->data[sensor->data_count] = value;
    sensor->data_count++;
    
    return true;
}
```

### Solution - get_stats

```c
void good_get_stats(const StaticSensor *sensor, 
                    double *avg, int *min, int *max) {
    if (sensor == NULL || avg == NULL || min == NULL || max == NULL) {
        return;
    }
    
    if (sensor->data_count == 0) {
        *avg = 0.0;
        *min = 0;
        *max = 0;
        return;
    }
    
    *min = sensor->data[0];
    *max = sensor->data[0];
    long sum = 0;
    
    for (size_t i = 0; i < sensor->data_count; i++) {
        sum += sensor->data[i];
        
        if (sensor->data[i] < *min) {
            *min = sensor->data[i];
        }
        if (sensor->data[i] > *max) {
            *max = sensor->data[i];
        }
    }
    
    *avg = (double)sum / sensor->data_count;
}
```

---

## Testing Your Solutions

### Compilation

```bash
cd exercises/

# Compile all
gcc -Wall -Wextra -Werror -std=c11 ex01_control_flow.c -o ex01
gcc -Wall -Wextra -Werror -std=c11 ex02_loop_bounds.c -o ex02
gcc -Wall -Wextra -Werror -std=c11 ex03_static_memory.c -o ex03

# Run
./ex01
./ex02
./ex03
```

### Validation Checklist

**Exercise 1:**
- [ ] No goto statements
- [ ] No recursion
- [ ] Compiles without warnings
---

## Exercise 4: Function Size

Le but est de décomposer une fonction de 100+ lignes en fonctions < 60 lignes.

### Solution - Fonctions individuelles

```c
uint32_t calculate_checksum(const uint8_t *data, size_t size) {
    assert(data != NULL);
    assert(size <= MAX_PACKET_SIZE);
    
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum ^ 0xFFFFFFFF;
}

bool validate_packet(Packet *packet) {
    assert(packet != NULL);
    
    if (packet->size > MAX_PACKET_SIZE) {
        packet->valid = false;
        return false;
    }
    
    uint32_t computed = calculate_checksum(packet->data, packet->size);
    packet->valid = (computed == packet->checksum);
    
    return packet->valid;
}

int validate_all_packets(Packet *packets, size_t count) {
    int valid_count = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (validate_packet(&packets[i])) {
            valid_count++;
        }
    }
    
    return valid_count;
}

void swap_packets(Packet *a, Packet *b) {
    Packet temp = *a;
    *a = *b;
    *b = temp;
}

void sort_packets_by_id(Packet *packets, size_t count) {
    for (size_t i = 0; i < count - 1; i++) {
        for (size_t j = 0; j < count - i - 1; j++) {
            if (packets[j].id > packets[j + 1].id) {
                swap_packets(&packets[j], &packets[j + 1]);
            }
        }
    }
}

int mark_duplicate_packets(Packet *packets, size_t count) {
    int duplicates = 0;
    
    for (size_t i = 0; i < count - 1; i++) {
        if (packets[i].id == packets[i + 1].id) {
            duplicates++;
            packets[i + 1].valid = false;
        }
    }
    
    return duplicates;
}

PacketStats calculate_packet_stats(const Packet *packets, size_t count) {
    PacketStats stats = {
        .valid_count = 0,
        .total_bytes = 0,
        .min_id = 0xFFFF,
        .max_id = 0
    };
    
    for (size_t i = 0; i < count; i++) {
        if (packets[i].valid) {
            stats.valid_count++;
            stats.total_bytes += packets[i].size;
            
            if (packets[i].id < stats.min_id) {
                stats.min_id = packets[i].id;
            }
            if (packets[i].id > stats.max_id) {
                stats.max_id = packets[i].id;
            }
        }
    }
    
    return stats;
}

void print_packet_report(size_t total, int duplicates, 
                         const PacketStats *stats) {
    printf("=== Packet Processing Report ===\n");
    printf("Total packets: %zu\n", total);
    printf("Valid packets: %d\n", stats->valid_count);
    printf("Invalid packets: %zu\n", total - stats->valid_count);
    printf("Duplicates found: %d\n", duplicates);
    printf("Total bytes: %zu\n", stats->total_bytes);
    printf("ID range: %u - %u\n", stats->min_id, stats->max_id);
}

int good_process_packets(Packet *packets, size_t count) {
    if (packets == NULL || count == 0) {
        return -1;
    }
    
    validate_all_packets(packets, count);
    sort_packets_by_id(packets, count);
    int duplicates = mark_duplicate_packets(packets, count);
    PacketStats stats = calculate_packet_stats(packets, count);
    print_packet_report(count, duplicates, &stats);
    
    return stats.valid_count;
}
```

**Clés**: Chaque fonction fait UNE chose, noms descriptifs, orchestration simple.

---

## Exercise 5: Check Returns

### Solution - Vérification complète des retours

```c
ErrorCode good_copy_file(const char *src, const char *dest) {
    if (src == NULL || dest == NULL) {
        return ERR_NULL_POINTER;
    }
    
    FILE *source = fopen(src, "r");
    if (source == NULL) {
        return ERR_FILE_OPEN;
    }
    
    FILE *destination = fopen(dest, "w");
    if (destination == NULL) {
        fclose(source);
        return ERR_FILE_OPEN;
    }
    
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, source);
    
    if (ferror(source)) {
        fclose(source);
        fclose(destination);
        return ERR_FILE_READ;
    }
    
    size_t bytes_written = fwrite(buffer, 1, bytes_read, destination);
    if (bytes_written != bytes_read) {
        fclose(source);
        fclose(destination);
        return ERR_FILE_WRITE;
    }
    
    fclose(source);
    fclose(destination);
    
    return ERR_OK;
}

ErrorCode good_string_operations(char *dest, size_t dest_size, 
                                  const char *src, int *out_value) {
    if (dest == NULL || src == NULL || out_value == NULL) {
        return ERR_NULL_POINTER;
    }
    
    if (dest_size == 0) {
        return ERR_BUFFER_OVERFLOW;
    }
    
    // Safe string copy
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
    
    // Safe string to int conversion
    char *endptr;
    errno = 0;
    long value = strtol(src, &endptr, 10);
    
    if (errno == ERANGE || endptr == src || *endptr != '\0') {
        return ERR_INVALID_DATA;
    }
    
    *out_value = (int)value;
    return ERR_OK;
}

const char* error_to_string(ErrorCode error) {
    switch (error) {
        case ERR_OK:
            return "Success";
        case ERR_NULL_POINTER:
            return "Null pointer";
        case ERR_FILE_OPEN:
            return "Cannot open file";
        case ERR_FILE_READ:
            return "Cannot read file";
        case ERR_FILE_WRITE:
            return "Cannot write file";
        case ERR_BUFFER_OVERFLOW:
            return "Buffer overflow";
        case ERR_INVALID_DATA:
            return "Invalid data";
        default:
            return "Unknown error";
    }
}
```

---

## Exercise 6: Limit Scope

### Solution - Portée minimale

```c
void good_data_processing(int *data, size_t size) {
    // Sum in minimal scope
    {
        int sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += data[i];
        }
        double average = (double)sum / size;
        printf("Average: %.2f\n", average);
    }
    
    // Max in minimal scope
    {
        int max = data[0];
        for (size_t i = 1; i < size; i++) {
            if (data[i] > max) {
                max = data[i];
            }
        }
        
        int min = data[0];
        for (size_t i = 1; i < size; i++) {
            if (data[i] < min) {
                min = data[i];
            }
        }
        
        int range = max - min;
        printf("Range: %d\n", range);
    }
    
    // Sort for median
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                int temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
    
    int median = data[size / 2];
    printf("Median: %d\n", median);
}

void good_read_sensor(SensorState *state) {
    assert(state != NULL);
    
    state->reading_count++;
    state->last_value = 25.5 + state->sensor_id;
    
    if (!state->calibrated) {
        printf("Sensor %d not calibrated!\n", state->sensor_id);
    }
}

void good_minimal_scope(int *array, size_t size) {
    // Sort with properly scoped swap variable
    for (size_t i = 0; i < size - 1; i++) {
        if (array[i] > array[i + 1]) {
            int swap_temp = array[i];
            array[i] = array[i + 1];
            array[i + 1] = swap_temp;
        }
    }
    
    // Calculate sum with its own variable
    {
        int sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += array[i];
        }
        printf("Sum: %d\n", sum);
    }
    
    // Find minimum with its own variable
    {
        int minimum = array[0];
        for (size_t i = 1; i < size; i++) {
            if (array[i] < minimum) {
                minimum = array[i];
            }
        }
        printf("Min: %d\n", minimum);
    }
}
```

---

## Exercise 7: Assertions

### Solution - Assertions complètes

```c
void good_queue_init(CircularQueue *queue) {
    assert(queue != NULL);
    
    memset(queue, 0, sizeof(CircularQueue));
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    
    assert(queue->count == 0);  // Postcondition
}

bool good_queue_is_full(const CircularQueue *queue) {
    assert(queue != NULL);
    return queue->count >= MAX_QUEUE_SIZE;
}

bool good_queue_is_empty(const CircularQueue *queue) {
    assert(queue != NULL);
    return queue->count == 0;
}

void good_queue_enqueue(CircularQueue *queue, int value) {
    assert(queue != NULL);
    assert(!good_queue_is_full(queue));  // Precondition
    assert(queue->count < MAX_QUEUE_SIZE);
    
    queue->items[queue->tail] = value;
    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    queue->count++;
    
    assert(queue->count <= MAX_QUEUE_SIZE);  // Postcondition
}

int good_queue_dequeue(CircularQueue *queue) {
    assert(queue != NULL);
    assert(!good_queue_is_empty(queue));  // Precondition
    assert(queue->count > 0);
    
    int value = queue->items[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    queue->count--;
    
    assert(queue->count < MAX_QUEUE_SIZE);  // Postcondition
    return value;
}

void good_array_set(int *array, size_t size, size_t index, int value) {
    assert(array != NULL);
    assert(size > 0);
    assert(index < size);
    
    array[index] = value;
}

bool good_account_is_valid(const Account *account) {
    if (account == NULL) {
        return false;
    }
    
    if (account->age < 0 || account->age > 150) {
        return false;
    }
    
    if (account->balance < 0.0) {
        return false;
    }
    
    if (account->name[0] == '\0') {
        return false;
    }
    
    return true;
}

void good_create_account(Account *account, const char *name, int age) {
    assert(account != NULL);
    assert(name != NULL);
    assert(name[0] != '\0');
    assert(age >= 0 && age <= 150);
    
    strncpy(account->name, name, MAX_NAME_LENGTH - 1);
    account->name[MAX_NAME_LENGTH - 1] = '\0';
    account->age = age;
    account->balance = 0.0;
    account->active = true;
    
    assert(good_account_is_valid(account));  // Postcondition
}
```

---

## Exercise 8: Pointer Indirection

### Solution - Maximum 2 niveaux

```c
void good_array3d_init(Array3D *array, size_t x, size_t y, size_t z) {
    assert(array != NULL);
    
    array->width = x;
    array->height = y;
    array->depth = z;
    
    size_t total_size = x * y * z;
    array->data = calloc(total_size, sizeof(int));
    assert(array->data != NULL);
}

void good_array3d_set(Array3D *array, size_t x, size_t y, size_t z, int value) {
    assert(array != NULL);
    assert(array->data != NULL);
    assert(x < array->width);
    assert(y < array->height);
    assert(z < array->depth);
    
    size_t index = x * (array->height * array->depth) + y * array->depth + z;
    array->data[index] = value;
}

int good_array3d_get(const Array3D *array, size_t x, size_t y, size_t z) {
    assert(array != NULL);
    assert(array->data != NULL);
    assert(x < array->width);
    assert(y < array->height);
    assert(z < array->depth);
    
    size_t index = x * (array->height * array->depth) + y * array->depth + z;
    return array->data[index];
}

void good_tree_add_child(SimpleTreeNode *parent, SimpleTreeNode *child) {
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent->child_count < MAX_NODES);
    
    parent->children[parent->child_count] = child;
    parent->child_count++;
}

void good_indexed_tree_init(IndexedTree *tree) {
    assert(tree != NULL);
    
    memset(tree, 0, sizeof(IndexedTree));
    
    for (size_t i = 0; i < MAX_NODES; i++) {
        tree->nodes[i].parent_index = -1;
        tree->nodes[i].first_child_index = -1;
        tree->nodes[i].next_sibling_index = -1;
        tree->nodes[i].active = false;
    }
}

int good_indexed_tree_add_node(IndexedTree *tree, int value, int parent_idx) {
    assert(tree != NULL);
    assert(tree->count < MAX_NODES);
    
    int node_idx = (int)tree->count;
    IndexedTreeNode *node = &tree->nodes[node_idx];
    
    node->value = value;
    node->parent_index = parent_idx;
    node->active = true;
    
    tree->count++;
    
    return node_idx;
}
```

---

## Exercise 9: Zero Warnings

### Solution - Code propre

```c
int good_initialized(void) {
    int x = 0;  // Initialized
    return x * 2;
}

void good_no_unused(void) {
    int used = 10;
    printf("%d\n", used);
    // No unused variable
}

void good_unused_parameter(int x, int y) {
    (void)y;  // Mark as intentionally unused
    printf("%d\n", x);
}

int good_all_paths_return(int x) {
    if (x > 0) {
        return x;
    }
    return 0;  // All paths return
}

void good_sign_comparison(void) {
    int signed_val = -1;
    size_t unsigned_val = 10;
    
    // Cast to same type
    if (signed_val >= 0 && (size_t)signed_val < unsigned_val) {
        printf("Less\n");
    }
}

void good_explicit_conversion(void) {
    double d = 3.14159;
    int i = (int)d;  // Explicit cast
    printf("%d\n", i);
}

void good_switch_complete(State state) {
    switch (state) {
        case STATE_INIT:
            printf("Init\n");
            break;
        case STATE_RUNNING:
            printf("Running\n");
            break;
        case STATE_PAUSED:
            printf("Paused\n");
            break;
        case STATE_STOPPED:
            printf("Stopped\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }
}

void good_format_string(void) {
    size_t size = 100;
    int value = 42;
    
    printf("Size: %zu\n", size);   // Correct format for size_t
    printf("Value: %d\n", value);  // Correct format for int
}

void good_pointer_arithmetic(void *ptr, size_t offset) {
    if (ptr != NULL) {
        char *byte_ptr = (char *)ptr;
        byte_ptr += offset;
        printf("%p\n", (void *)byte_ptr);
    }
}

void good_comparison(int x) {
    if (x == 5) {  // Use == not =
        printf("Equal\n");
    }
}
```

---

## Exercise 10: Static Analysis

### Solution - Code sûr

```c
bool good_safe_assign(int *ptr, int value) {
    if (ptr == NULL) {
        return false;
    }
    *ptr = value;
    return true;
}

bool good_no_leak(size_t size) {
    int *data = malloc(size * sizeof(int));
    if (data == NULL) {
        return false;
    }
    
    bool success = true;
    
    for (size_t i = 0; i < size; i++) {
        data[i] = (int)i;
    }
    
    if (size > 100) {
        success = false;
        // Don't return yet - cleanup first!
    }
    
    free(data);  // Always freed
    return success;
}

bool good_safe_strcpy(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return false;
    }
    
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return false;  // Won't fit
    }
    
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
    
    return true;
}

void good_safe_free(int **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;  // Prevent use-after-free
    }
}

int* good_initialized_malloc(size_t count) {
    if (count == 0 || count > SIZE_MAX / sizeof(int)) {
        return NULL;
    }
    
    int *array = calloc(count, sizeof(int));  // Initialized to 0
    return array;  // Could be NULL, caller must check
}

bool good_safe_divide(int a, int b, int *result) {
    if (result == NULL) {
        return false;
    }
    
    if (b == 0) {
        return false;  // Division by zero
    }
    
    *result = a / b;
    return true;
}

void good_array_bounds(int *array, size_t size) {
    if (array == NULL || size == 0) {
        return;
    }
    
    for (size_t i = 0; i < size; i++) {  // Correct: i < size
        array[i] = (int)i;
    }
}
```

---

## Next Steps

Une fois ces exercices maîtrisés:

1. ✅ Appliquer les 10 règles ensemble dans un projet complet
2. 🔍 Utiliser les outils d'analyse statique régulièrement
3. 📚 Étudier les exemples détaillés dans `rule01_control_flow.c` à `rule10_static_analysis.c`
4. 🚀 Appliquer à vos propres projets critiques

## Ressources

- **NASA JPL Coding Standards**: Original paper des Power of 10 Rules
- **MISRA C**: Standard pour l'industrie automobile
- **CERT C Coding Standard**: Règles de sécurité pour C
- **Static Analysis Tools**: Clang, Cppcheck, Coverity

**Exercise 3:**
- [ ] No malloc/free
- [ ] Fixed-size buffers
- [ ] Pool working correctly
- [ ] Compiles without warnings
- [ ] Tests pass

---

## Next Steps

Une fois ces exercices maîtrisés:
1. Essayez Exercise 7 (système complet)
2. Faites le code review (Exercise 8)
3. Appliquez à vos propres projets
4. Étudiez les règles 4-10 en détail
