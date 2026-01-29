# Règle 3 : Pas d'Allocation Dynamique Après Init

## 📋 Énoncé

**Pas de malloc/free après l'initialisation**
- Toute allocation dynamique doit être faite pendant la phase d'initialisation
- Pas de `malloc`, `calloc`, `realloc`, `free` pendant l'exécution normale
- Pas d'allocation sur le tas en runtime
- Préférer allocation statique ou sur la pile

## 🎯 Objectif

Éliminer les risques de:
- **Fragmentation mémoire**
- **Fuites mémoire**
- **Échecs d'allocation imprévisibles**
- **Comportement non-déterministe**

## ❌ Problèmes

### 1. Allocation en runtime

```c
// ❌ MAUVAIS
void process_sensor_data(int sensor_id) {
    // Allocation pendant l'exécution!
    int *buffer = malloc(100 * sizeof(int));
    if (buffer == NULL) {
        return;  // Que faire?
    }
    
    read_sensor(sensor_id, buffer, 100);
    process_data(buffer, 100);
    
    free(buffer);
}
```

**Problèmes:**
- Échec d'allocation possible
- Fragmentation mémoire progressive
- Fuites si oubli de free
- Comportement imprévisible

### 2. Realloc dynamique

```c
// ❌ MAUVAIS
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynamicArray;

void array_push(DynamicArray *arr, int value) {
    if (arr->size >= arr->capacity) {
        // Croissance dynamique = mauvais!
        arr->capacity *= 2;
        arr->data = realloc(arr->data, arr->capacity * sizeof(int));
    }
    arr->data[arr->size++] = value;
}
```

**Problèmes:**
- Temps d'exécution imprévisible
- Peut échouer à tout moment
- Fragmentation

### 3. Structures dynamiques

```c
// ❌ MAUVAIS
typedef struct Node {
    int value;
    struct Node *next;
} Node;

Node* add_node(Node *head, int value) {
    Node *new_node = malloc(sizeof(Node));  // Runtime allocation!
    new_node->value = value;
    new_node->next = head;
    return new_node;
}
```

## ✅ Solutions

### 1. Allocation statique

```c
// ✅ BON - Buffer statique
#define MAX_SENSOR_SAMPLES 100

void process_sensor_data(int sensor_id) {
    static int buffer[MAX_SENSOR_SAMPLES];  // Statique!
    
    read_sensor(sensor_id, buffer, MAX_SENSOR_SAMPLES);
    process_data(buffer, MAX_SENSOR_SAMPLES);
}
```

### 2. Object Pool

```c
// ✅ BON - Pool pré-alloué
#define MAX_SENSORS 10

typedef struct {
    int id;
    int data[100];
    size_t data_count;
    bool active;
} Sensor;

typedef struct {
    Sensor sensors[MAX_SENSORS];
    size_t allocated_count;
} SensorPool;

static SensorPool g_sensor_pool = {0};

Sensor* sensor_pool_acquire(void) {
    // Chercher un sensor inactif
    for (size_t i = 0; i < MAX_SENSORS; i++) {
        if (!g_sensor_pool.sensors[i].active) {
            g_sensor_pool.sensors[i].active = true;
            g_sensor_pool.sensors[i].data_count = 0;
            g_sensor_pool.allocated_count++;
            return &g_sensor_pool.sensors[i];
        }
    }
    return NULL;  // Pool plein
}

void sensor_pool_release(Sensor *sensor) {
    if (sensor == NULL) return;
    
    // Vérifier que c'est bien du pool
    if (sensor < g_sensor_pool.sensors || 
        sensor >= g_sensor_pool.sensors + MAX_SENSORS) {
        return;
    }
    
    sensor->active = false;
    g_sensor_pool.allocated_count--;
}
```

### 3. Array fixe au lieu de liste chaînée

```c
// ✅ BON - Array-based list
#define MAX_NODES 50

typedef struct {
    int value;
    int next_index;  // -1 = fin
    bool active;
} Node;

typedef struct {
    Node nodes[MAX_NODES];
    int head_index;
    size_t count;
} StaticList;

int list_add(StaticList *list, int value) {
    // Trouver un nœud libre
    for (int i = 0; i < MAX_NODES; i++) {
        if (!list->nodes[i].active) {
            list->nodes[i].value = value;
            list->nodes[i].next_index = list->head_index;
            list->nodes[i].active = true;
            list->head_index = i;
            list->count++;
            return i;
        }
    }
    return -1;  // Liste pleine
}

void list_remove(StaticList *list, int index) {
    if (index < 0 || index >= MAX_NODES) return;
    if (!list->nodes[index].active) return;
    
    list->nodes[index].active = false;
    list->count--;
    
    // Réorganiser les liens
    if (list->head_index == index) {
        list->head_index = list->nodes[index].next_index;
    }
}
```

## 📊 Patterns Avancés

### Ring Buffer (pas d'allocation)

```c
#define BUFFER_SIZE 32

typedef struct {
    uint8_t data[BUFFER_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} RingBuffer;

bool ring_buffer_write(RingBuffer *buf, uint8_t value) {
    if (buf->count >= BUFFER_SIZE) {
        return false;  // Plein
    }
    
    buf->data[buf->tail] = value;
    buf->tail = (buf->tail + 1) % BUFFER_SIZE;
    buf->count++;
    return true;
}

bool ring_buffer_read(RingBuffer *buf, uint8_t *value) {
    if (buf->count == 0) {
        return false;  // Vide
    }
    
    *value = buf->data[buf->head];
    buf->head = (buf->head + 1) % BUFFER_SIZE;
    buf->count--;
    return true;
}
```

### Hash Table Statique

```c
#define HASH_TABLE_SIZE 64

typedef struct {
    int key;
    int value;
    bool occupied;
} HashEntry;

typedef struct {
    HashEntry entries[HASH_TABLE_SIZE];
} HashTable;

static HashTable g_hash_table = {0};

int hash_function(int key) {
    return (key & 0x7FFFFFFF) % HASH_TABLE_SIZE;
}

bool hash_table_insert(int key, int value) {
    int index = hash_function(key);
    int attempts = 0;
    
    // Linear probing
    while (attempts < HASH_TABLE_SIZE) {
        if (!g_hash_table.entries[index].occupied) {
            g_hash_table.entries[index].key = key;
            g_hash_table.entries[index].value = value;
            g_hash_table.entries[index].occupied = true;
            return true;
        }
        
        if (g_hash_table.entries[index].key == key) {
            // Update existing
            g_hash_table.entries[index].value = value;
            return true;
        }
        
        index = (index + 1) % HASH_TABLE_SIZE;
        attempts++;
    }
    
    return false;  // Table pleine
}

bool hash_table_get(int key, int *value) {
    int index = hash_function(key);
    int attempts = 0;
    
    while (attempts < HASH_TABLE_SIZE) {
        if (!g_hash_table.entries[index].occupied) {
            return false;  // Pas trouvé
        }
        
        if (g_hash_table.entries[index].key == key) {
            *value = g_hash_table.entries[index].value;
            return true;
        }
        
        index = (index + 1) % HASH_TABLE_SIZE;
        attempts++;
    }
    
    return false;
}
```

### Event Queue Statique

```c
#define MAX_EVENTS 32

typedef enum {
    EVENT_BUTTON_PRESS,
    EVENT_SENSOR_DATA,
    EVENT_TIMEOUT,
    EVENT_ERROR
} EventType;

typedef struct {
    EventType type;
    int data;
    uint32_t timestamp;
} Event;

typedef struct {
    Event events[MAX_EVENTS];
    size_t head;
    size_t tail;
    size_t count;
} EventQueue;

static EventQueue g_event_queue = {0};

bool event_queue_push(EventType type, int data) {
    if (g_event_queue.count >= MAX_EVENTS) {
        return false;  // Queue pleine
    }
    
    Event *event = &g_event_queue.events[g_event_queue.tail];
    event->type = type;
    event->data = data;
    event->timestamp = get_system_time();
    
    g_event_queue.tail = (g_event_queue.tail + 1) % MAX_EVENTS;
    g_event_queue.count++;
    
    return true;
}

bool event_queue_pop(Event *event) {
    if (g_event_queue.count == 0) {
        return false;  // Queue vide
    }
    
    *event = g_event_queue.events[g_event_queue.head];
    g_event_queue.head = (g_event_queue.head + 1) % MAX_EVENTS;
    g_event_queue.count--;
    
    return true;
}
```

### Memory Arena (init only)

```c
#define ARENA_SIZE (64 * 1024)  // 64 KB

typedef struct {
    uint8_t memory[ARENA_SIZE];
    size_t used;
    bool initialized;
} MemoryArena;

static MemoryArena g_arena = {0};

// Appelé UNE FOIS pendant l'initialisation
void arena_init(void) {
    g_arena.used = 0;
    g_arena.initialized = true;
}

// Allocation pendant init SEULEMENT
void* arena_alloc(size_t size) {
    assert(g_arena.initialized);
    
    // Alignement 8 bytes
    size = (size + 7) & ~7;
    
    if (g_arena.used + size > ARENA_SIZE) {
        return NULL;
    }
    
    void *ptr = &g_arena.memory[g_arena.used];
    g_arena.used += size;
    
    return ptr;
}

// Pas de free individuel!
// Toute la mémoire est réinitialisée ensemble si besoin
void arena_reset(void) {
    g_arena.used = 0;
}
```

## 🎓 Exercices

Voir `exercises/ex03_static_memory.c`

## 🔍 Détection

### Static Analysis

```bash
# Rechercher malloc/free/realloc
grep -n "malloc\|calloc\|realloc\|free" *.c

# Avec exclusion de l'init
grep -n "malloc\|calloc\|realloc\|free" *.c | grep -v "init"

# Clang analyzer
clang --analyze -Xanalyzer -analyzer-checker=unix.Malloc *.c
```

### Runtime Detection

```bash
# Valgrind pour fuites
valgrind --leak-check=full ./program

# Massif pour heap profiling
valgrind --tool=massif ./program
```

## 📚 Cas d'usage Real-World

### Mars Rover

```c
// Toutes les structures pré-allouées
#define MAX_WAYPOINTS 100
#define MAX_SAMPLES 50

typedef struct {
    Waypoint waypoints[MAX_WAYPOINTS];
    Sample samples[MAX_SAMPLES];
    SensorData sensor_buffer;
    CommandQueue commands;
} RoverState;

static RoverState g_rover = {0};  // Statique!

void rover_init(void) {
    // Initialisation une fois
    memset(&g_rover, 0, sizeof(g_rover));
}
```

## 💡 Points clés

1. **Pre-allocate everything**: Allouer tout pendant l'init
2. **Object pools**: Pour réutiliser les objets
3. **Fixed-size arrays**: Au lieu de structures dynamiques
4. **Ring buffers**: Pour queues et buffers circulaires
5. **Static globals**: Pour éviter la pile

## ⚠️ Pièges

```c
// ❌ Variable Length Array (VLA) - c'est sur la pile!
void bad_vla(int n) {
    int array[n];  // Stack overflow si n grand!
}

// ✅ Taille fixe
void good_fixed(int n) {
    int array[MAX_SIZE];
    int safe_n = (n > MAX_SIZE) ? MAX_SIZE : n;
    // Utiliser array[0..safe_n-1]
}
```

---

**Résumé**: Tout allouer pendant l'init, utiliser des pools statiques, pas de malloc/free en runtime.
