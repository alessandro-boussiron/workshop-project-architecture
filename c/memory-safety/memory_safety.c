/*
 * MEMORY SAFETY PATTERNS IN C
 * 
 * ═══════════════════════════════════════════════════════════════════════
 * RÈGLES FONDAMENTALES DE SÛRETÉ MÉMOIRE
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * RÈGLE 1: PRÉFÉRER L'ALLOCATION STATIQUE
 * ─────────────────────────────────────────
 * ⚠️  malloc() n'est PAS obligatoire!
 * ✅ 90% des cas peuvent utiliser:
 *    - Variables locales (stack)
 *    - Tableaux de taille fixe
 *    - Structures statiques
 *    - Allocation au démarrage uniquement
 * 
 * RÈGLE 2: TAILLE MAXIMALE CONNUE = TABLEAU FIXE
 * ───────────────────────────────────────────────
 * Si vous connaissez la taille max → utilisez un tableau statique
 * Exemple: MAX_USERS, MAX_BUFFER_SIZE, MAX_CONNECTIONS
 * 
 * RÈGLE 3: PAS DE malloc() APRÈS L'INITIALISATION
 * ────────────────────────────────────────────────
 * Pour systèmes critiques (NASA, avionique, spatial):
 * - Toute mémoire allouée au démarrage
 * - Pas d'allocation dynamique pendant l'exécution
 * - Comportement déterministe garanti
 * 
 * RÈGLE 4: OWNERSHIP CLAIR
 * ─────────────────────────
 * Chaque pointeur a UN propriétaire qui:
 * - Connaît sa durée de vie
 * - Est responsable de sa libération
 * - Documente cette responsabilité
 * 
 * RÈGLE 5: DÉFENSE EN PROFONDEUR
 * ───────────────────────────────
 * - Vérifier les bornes TOUJOURS
 * - Initialiser TOUTES les variables
 * - NULL checks systématiques
 * - Assertions libérales
 * - Outils de validation (ASan, Valgrind)
 * 
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * Compilation recommandée:
 * gcc -Wall -Wextra -Werror -pedantic -std=c11 -g -fsanitize=address memory_safety.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// ═══════════════════════════════════════════════════════════════════════
// PATTERN 0: ALLOCATION STATIQUE (LE PLUS SÛR)
// Pas de malloc nécessaire dans la plupart des cas!
// ═══════════════════════════════════════════════════════════════════════

/* ❌ MAUVAIS - Allocation dynamique inutile */
typedef struct {
    int *data;
    size_t size;
} BadBuffer;

BadBuffer* bad_create_buffer(size_t size) {
    BadBuffer *buf = malloc(sizeof(BadBuffer));
    if (buf == NULL) return NULL;
    
    buf->data = malloc(size * sizeof(int));
    if (buf->data == NULL) {
        free(buf);
        return NULL;
    }
    
    buf->size = size;
    return buf;  // PROBLÈME: Qui libère? Quand? Comment?
}

/* ✅ EXCELLENT - Allocation statique, pas de malloc! */
#define STATIC_BUFFER_SIZE 1024

typedef struct {
    int data[STATIC_BUFFER_SIZE];  // ✅ Tableau statique
    size_t count;                   // Nombre d'éléments utilisés
} GoodBuffer;

// Initialisation simple, pas de malloc
void good_buffer_init(GoodBuffer *buf) {
    assert(buf != NULL);
    memset(buf, 0, sizeof(GoodBuffer));
}

// Ajout sécurisé avec vérification de bornes
bool good_buffer_add(GoodBuffer *buf, int value) {
    assert(buf != NULL);
    
    if (buf->count >= STATIC_BUFFER_SIZE) {
        fprintf(stderr, "Buffer full (max: %d)\n", STATIC_BUFFER_SIZE);
        return false;
    }
    
    buf->data[buf->count] = value;
    buf->count++;
    return true;
}

// Pas de fonction destroy nécessaire! Aucune fuite possible!

void static_allocation_example(void) {
    printf("═══════════════════════════════════════════════════\n");
    printf("✨ PATTERN 0: ALLOCATION STATIQUE (Pas de malloc!)\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    // ✅ Allocation sur la stack - automatique et sûre
    GoodBuffer buffer;
    good_buffer_init(&buffer);
    
    printf("📦 Buffer statique créé (taille max: %d)\n", STATIC_BUFFER_SIZE);
    
    // Ajout de données
    for (int i = 0; i < 10; i++) {
        good_buffer_add(&buffer, i * 10);
    }
    
    printf("✅ %zu éléments ajoutés\n", buffer.count);
    printf("✅ Aucun malloc utilisé!\n");
    printf("✅ Aucune fuite possible!\n");
    printf("✅ Déterministe et prévisible!\n\n");
    
    // Pas de cleanup nécessaire - automatique quand la fonction se termine!
}

// ═══════════════════════════════════════════════════════════════════════
// PATTERN 0.1: STRUCTURES FIXES POUR SYSTÈMES EMBARQUÉS
// Exemple: File de messages sans malloc
// ═══════════════════════════════════════════════════════════════════════

#define MAX_MESSAGES 64
#define MESSAGE_SIZE 128

typedef struct {
    char text[MESSAGE_SIZE];
    uint32_t timestamp;
    uint8_t priority;
} Message;

typedef struct {
    Message messages[MAX_MESSAGES];  // ✅ Tableau fixe, pas de malloc!
    size_t head;
    size_t tail;
    size_t count;
} MessageQueue;

// Initialisation O(1) - pas de malloc
void msg_queue_init(MessageQueue *queue) {
    assert(queue != NULL);
    memset(queue, 0, sizeof(MessageQueue));
}

// Enqueue - vérifie les bornes
bool msg_queue_push(MessageQueue *queue, const char *text, uint8_t priority) {
    assert(queue != NULL);
    assert(text != NULL);
    
    if (queue->count >= MAX_MESSAGES) {
        fprintf(stderr, "Queue full!\n");
        return false;
    }
    
    Message *msg = &queue->messages[queue->tail];
    strncpy(msg->text, text, MESSAGE_SIZE - 1);
    msg->text[MESSAGE_SIZE - 1] = '\0';
    msg->timestamp = (uint32_t)queue->count; // Simulé
    msg->priority = priority;
    
    queue->tail = (queue->tail + 1) % MAX_MESSAGES;
    queue->count++;
    
    return true;
}

// Dequeue - vérifie si vide
bool msg_queue_pop(MessageQueue *queue, Message *out) {
    assert(queue != NULL);
    assert(out != NULL);
    
    if (queue->count == 0) {
        return false;
    }
    
    *out = queue->messages[queue->head];
    queue->head = (queue->head + 1) % MAX_MESSAGES;
    queue->count--;
    
    return true;
}

void message_queue_example(void) {
    printf("═══════════════════════════════════════════════════\n");
    printf("📬 EXEMPLE: File de messages sans malloc\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    MessageQueue queue;
    msg_queue_init(&queue);
    
    // Enqueue plusieurs messages
    msg_queue_push(&queue, "System started", 1);
    msg_queue_push(&queue, "Sensor data ready", 2);
    msg_queue_push(&queue, "Warning: temperature high", 3);
    
    printf("✅ 3 messages ajoutés (aucun malloc utilisé)\n");
    
    // Dequeue et affichage
    Message msg;
    while (msg_queue_pop(&queue, &msg)) {
        printf("   [Priority %d] %s\n", msg.priority, msg.text);
    }
    
    printf("\n✨ Avantages:\n");
    printf("   • Pas de malloc → pas de fuite possible\n");
    printf("   • Déterministe → temps d'exécution prévisible\n");
    printf("   • Simple → moins de bugs\n");
    printf("   • Performant → pas de fragmentation\n\n");
}

// ═══════════════════════════════════════════════════════════════════════
// PATTERN 0.2: HASH TABLE STATIQUE
// Exemple: Table de symboles sans allocation dynamique
// ═══════════════════════════════════════════════════════════════════════

#define HASH_TABLE_SIZE 128
#define KEY_SIZE 32
#define VALUE_SIZE 64

typedef struct {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    bool occupied;
} HashEntry;

typedef struct {
    HashEntry entries[HASH_TABLE_SIZE];  // ✅ Statique!
    size_t count;
} HashTable;

// Hash function simple
static uint32_t hash_string(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (uint32_t)c;
    }
    return hash % HASH_TABLE_SIZE;
}

void hash_table_init(HashTable *table) {
    assert(table != NULL);
    memset(table, 0, sizeof(HashTable));
}

bool hash_table_insert(HashTable *table, const char *key, const char *value) {
    assert(table != NULL);
    assert(key != NULL);
    assert(value != NULL);
    
    uint32_t index = hash_string(key);
    uint32_t probe = 0;
    
    // Linear probing
    while (probe < HASH_TABLE_SIZE) {
        uint32_t current = (index + probe) % HASH_TABLE_SIZE;
        
        if (!table->entries[current].occupied) {
            strncpy(table->entries[current].key, key, KEY_SIZE - 1);
            table->entries[current].key[KEY_SIZE - 1] = '\0';
            
            strncpy(table->entries[current].value, value, VALUE_SIZE - 1);
            table->entries[current].value[VALUE_SIZE - 1] = '\0';
            
            table->entries[current].occupied = true;
            table->count++;
            return true;
        }
        
        // Key already exists - update
        if (strcmp(table->entries[current].key, key) == 0) {
            strncpy(table->entries[current].value, value, VALUE_SIZE - 1);
            table->entries[current].value[VALUE_SIZE - 1] = '\0';
            return true;
        }
        
        probe++;
    }
    
    fprintf(stderr, "Hash table full!\n");
    return false;
}

bool hash_table_get(const HashTable *table, const char *key, char *out_value, size_t out_size) {
    assert(table != NULL);
    assert(key != NULL);
    assert(out_value != NULL);
    
    uint32_t index = hash_string(key);
    uint32_t probe = 0;
    
    while (probe < HASH_TABLE_SIZE) {
        uint32_t current = (index + probe) % HASH_TABLE_SIZE;
        
        if (!table->entries[current].occupied) {
            return false;  // Not found
        }
        
        if (strcmp(table->entries[current].key, key) == 0) {
            strncpy(out_value, table->entries[current].value, out_size - 1);
            out_value[out_size - 1] = '\0';
            return true;
        }
        
        probe++;
    }
    
    return false;
}

void hash_table_example(void) {
    printf("═══════════════════════════════════════════════════\n");
    printf("🗂️  EXEMPLE: Hash table sans malloc\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    HashTable table;
    hash_table_init(&table);
    
    // Insertions
    hash_table_insert(&table, "name", "Mars Rover");
    hash_table_insert(&table, "mission", "Perseverance");
    hash_table_insert(&table, "status", "Active");
    hash_table_insert(&table, "location", "Jezero Crater");
    
    printf("✅ 4 entrées insérées (aucun malloc!)\n\n");
    
    // Lookups
    char value[VALUE_SIZE];
    const char *keys[] = {"name", "mission", "status", "location"};
    
    for (size_t i = 0; i < 4; i++) {
        if (hash_table_get(&table, keys[i], value, sizeof(value))) {
            printf("   %s: %s\n", keys[i], value);
        }
    }
    
    printf("\n✨ Structure complète stockée statiquement!\n");
    printf("   • Capacité: %d entrées\n", HASH_TABLE_SIZE);
    printf("   • Utilisé: %zu entrées\n", table.count);
    printf("   • Mémoire: %zu bytes (stack)\n\n", sizeof(HashTable));
}

// ============================================
// PATTERN 1: ARENA ALLOCATOR
// Single allocation/deallocation point
// ============================================

#define ARENA_SIZE 4096

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t used;
    bool owns_buffer;
} Arena;

/* Initialize arena with pre-allocated buffer */
Arena* arena_create(size_t capacity) {
    Arena *arena = malloc(sizeof(Arena));
    if (arena == NULL) {
        return NULL;
    }
    
    arena->buffer = malloc(capacity);
    if (arena->buffer == NULL) {
        free(arena);
        return NULL;
    }
    
    arena->capacity = capacity;
    arena->used = 0;
    arena->owns_buffer = true;
    
    return arena;
}

/* Allocate from arena (no individual free needed) */
void* arena_alloc(Arena *arena, size_t size) {
    if (arena == NULL || size == 0) {
        return NULL;
    }
    
    // Align to 8 bytes
    size = (size + 7) & ~7;
    
    if (arena->used + size > arena->capacity) {
        fprintf(stderr, "Arena out of memory\n");
        return NULL;
    }
    
    void *ptr = arena->buffer + arena->used;
    arena->used += size;
    
    return ptr;
}

/* Reset arena without freeing (reuse memory) */
void arena_reset(Arena *arena) {
    if (arena != NULL) {
        arena->used = 0;
    }
}

/* Free entire arena at once */
void arena_destroy(Arena *arena) {
    if (arena == NULL) {
        return;
    }
    
    if (arena->owns_buffer) {
        free(arena->buffer);
    }
    free(arena);
}

/* Example usage of arena */
void arena_example(void) {
    printf("📦 Arena Allocator Pattern\n");
    
    Arena *arena = arena_create(ARENA_SIZE);
    assert(arena != NULL);
    
    // Allocate multiple objects
    int *numbers = arena_alloc(arena, 10 * sizeof(int));
    char *message = arena_alloc(arena, 100);
    
    if (numbers != NULL && message != NULL) {
        for (int i = 0; i < 10; i++) {
            numbers[i] = i * 10;
        }
        snprintf(message, 100, "Arena allocation successful");
        
        printf("  Numbers: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n  Message: %s\n", message);
    }
    
    // Single deallocation for everything
    arena_destroy(arena);
    printf("  ✅ Arena destroyed (all memory freed)\n\n");
}

// ============================================
// PATTERN 2: OBJECT POOL
// Pre-allocated fixed-size objects
// ============================================

#define POOL_SIZE 32

typedef struct {
    int id;
    char data[64];
    bool in_use;
} PoolObject;

typedef struct {
    PoolObject objects[POOL_SIZE];
    size_t allocated_count;
} ObjectPool;

/* Initialize pool */
void pool_init(ObjectPool *pool) {
    assert(pool != NULL);
    
    memset(pool, 0, sizeof(ObjectPool));
    for (size_t i = 0; i < POOL_SIZE; i++) {
        pool->objects[i].in_use = false;
    }
}

/* Acquire object from pool */
PoolObject* pool_acquire(ObjectPool *pool) {
    assert(pool != NULL);
    
    for (size_t i = 0; i < POOL_SIZE; i++) {
        if (!pool->objects[i].in_use) {
            pool->objects[i].in_use = true;
            pool->objects[i].id = (int)i;
            pool->allocated_count++;
            return &pool->objects[i];
        }
    }
    
    fprintf(stderr, "Pool exhausted\n");
    return NULL;
}

/* Release object back to pool */
void pool_release(ObjectPool *pool, PoolObject *obj) {
    assert(pool != NULL);
    assert(obj != NULL);
    
    // Verify object is from this pool
    if (obj < pool->objects || obj >= pool->objects + POOL_SIZE) {
        fprintf(stderr, "Object not from this pool\n");
        return;
    }
    
    if (!obj->in_use) {
        fprintf(stderr, "Double free detected\n");
        return;
    }
    
    // Clear and mark as free
    memset(obj, 0, sizeof(PoolObject));
    obj->in_use = false;
    pool->allocated_count--;
}

/* Example usage of object pool */
void pool_example(void) {
    printf("🏊 Object Pool Pattern\n");
    
    ObjectPool pool;
    pool_init(&pool);
    
    // Acquire objects
    PoolObject *obj1 = pool_acquire(&pool);
    PoolObject *obj2 = pool_acquire(&pool);
    
    if (obj1 != NULL && obj2 != NULL) {
        snprintf(obj1->data, sizeof(obj1->data), "Object 1 data");
        snprintf(obj2->data, sizeof(obj2->data), "Object 2 data");
        
        printf("  Acquired: %s, %s\n", obj1->data, obj2->data);
        printf("  Pool usage: %zu/%d\n", pool.allocated_count, POOL_SIZE);
        
        // Release
        pool_release(&pool, obj1);
        pool_release(&pool, obj2);
        
        printf("  ✅ Objects released, pool usage: %zu/%d\n\n",
               pool.allocated_count, POOL_SIZE);
    }
}

// ============================================
// PATTERN 3: RAII-LIKE WITH CLEANUP
// Goto cleanup pattern for resource management
// ============================================

typedef struct {
    FILE *log_file;
    int socket_fd;
    void *buffer;
} Resources;

/* ❌ BAD: Multiple return paths, resource leaks */
int bad_resource_management(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return -1;
    
    char *buffer = malloc(1024);
    if (buffer == NULL) return -1;  // LEAK: file not closed
    
    int fd = 1;  // Simulate socket
    if (fd < 0) return -1;  // LEAK: file and buffer
    
    // Process...
    
    free(buffer);
    fclose(file);
    return 0;
}

/* ✅ GOOD: Single cleanup point */
int good_resource_management(const char *filename) {
    int result = -1;
    FILE *file = NULL;
    char *buffer = NULL;
    int fd = -1;
    
    // Acquire resources
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        goto cleanup;
    }
    
    buffer = malloc(1024);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate buffer\n");
        goto cleanup;
    }
    
    fd = 1;  // Simulate socket
    if (fd < 0) {
        fprintf(stderr, "Failed to open socket\n");
        goto cleanup;
    }
    
    // Process...
    size_t bytes_read = fread(buffer, 1, 1024, file);
    if (bytes_read > 0) {
        result = 0;  // Success
    }
    
cleanup:
    // Single cleanup point - always executed
    if (buffer != NULL) {
        free(buffer);
    }
    if (file != NULL) {
        fclose(file);
    }
    if (fd >= 0) {
        // close(fd);  // In real code
    }
    
    return result;
}

void cleanup_pattern_example(void) {
    printf("🧹 Cleanup Pattern (RAII-like)\n");
    
    // Create test file
    FILE *test = fopen("test.txt", "w");
    if (test != NULL) {
        fprintf(test, "Test data");
        fclose(test);
    }
    
    int result = good_resource_management("test.txt");
    printf("  Resource management result: %d\n", result);
    printf("  ✅ All resources cleaned up\n\n");
    
    remove("test.txt");
}

// ============================================
// PATTERN 4: BOUNDS CHECKING
// Always validate array/buffer access
// ============================================

typedef struct {
    int *data;
    size_t capacity;
    size_t size;
} SafeArray;

/* Initialize array */
bool safe_array_init(SafeArray *array, size_t capacity) {
    assert(array != NULL);
    assert(capacity > 0);
    
    array->data = calloc(capacity, sizeof(int));
    if (array->data == NULL) {
        return false;
    }
    
    array->capacity = capacity;
    array->size = 0;
    return true;
}

/* Bounds-checked set */
bool safe_array_set(SafeArray *array, size_t index, int value) {
    assert(array != NULL);
    
    if (index >= array->capacity) {
        fprintf(stderr, "Index %zu out of bounds (capacity: %zu)\n",
                index, array->capacity);
        return false;
    }
    
    array->data[index] = value;
    if (index >= array->size) {
        array->size = index + 1;
    }
    
    return true;
}

/* Bounds-checked get */
bool safe_array_get(const SafeArray *array, size_t index, int *out_value) {
    assert(array != NULL);
    assert(out_value != NULL);
    
    if (index >= array->size) {
        fprintf(stderr, "Index %zu out of bounds (size: %zu)\n",
                index, array->size);
        return false;
    }
    
    *out_value = array->data[index];
    return true;
}

/* Cleanup */
void safe_array_destroy(SafeArray *array) {
    if (array != NULL && array->data != NULL) {
        free(array->data);
        array->data = NULL;
        array->capacity = 0;
        array->size = 0;
    }
}

void bounds_checking_example(void) {
    printf("🔒 Bounds Checking Pattern\n");
    
    SafeArray array;
    if (!safe_array_init(&array, 10)) {
        fprintf(stderr, "Failed to initialize array\n");
        return;
    }
    
    // Safe operations
    safe_array_set(&array, 0, 100);
    safe_array_set(&array, 5, 500);
    
    int value;
    if (safe_array_get(&array, 5, &value)) {
        printf("  array[5] = %d\n", value);
    }
    
    // This will fail safely
    printf("  Attempting out-of-bounds access:\n  ");
    safe_array_get(&array, 100, &value);
    
    safe_array_destroy(&array);
    printf("  ✅ Array destroyed safely\n\n");
}

// ============================================
// PATTERN 5: SAFE STRING OPERATIONS
// Avoid buffer overflows
// ============================================

#define MAX_STRING_LEN 256

/* ❌ BAD: Unsafe string operations */
void bad_string_ops(void) {
    char buffer[10];
    char *input = "This is a very long string that will overflow";
    
    strcpy(buffer, input);  // BUFFER OVERFLOW!
    strcat(buffer, " more");  // BUFFER OVERFLOW!
}

/* ✅ GOOD: Safe string operations */
typedef struct {
    char data[MAX_STRING_LEN];
    size_t length;
} SafeString;

bool safe_string_set(SafeString *str, const char *value) {
    assert(str != NULL);
    
    if (value == NULL) {
        str->data[0] = '\0';
        str->length = 0;
        return true;
    }
    
    // Use safe string copy
    strncpy(str->data, value, MAX_STRING_LEN - 1);
    str->data[MAX_STRING_LEN - 1] = '\0';  // Ensure null termination
    str->length = strnlen(str->data, MAX_STRING_LEN);
    
    return true;
}

bool safe_string_append(SafeString *str, const char *suffix) {
    assert(str != NULL);
    assert(suffix != NULL);
    
    size_t suffix_len = strnlen(suffix, MAX_STRING_LEN);
    size_t available = MAX_STRING_LEN - str->length - 1;
    
    if (suffix_len > available) {
        fprintf(stderr, "String append would overflow\n");
        return false;
    }
    
    strncat(str->data, suffix, available);
    str->length = strnlen(str->data, MAX_STRING_LEN);
    
    return true;
}

void safe_string_example(void) {
    printf("📝 Safe String Operations\n");
    
    SafeString str;
    safe_string_set(&str, "Hello");
    printf("  Initial: '%s' (len: %zu)\n", str.data, str.length);
    
    safe_string_append(&str, " World");
    printf("  After append: '%s' (len: %zu)\n", str.data, str.length);
    
    // Try overflow
    char long_str[MAX_STRING_LEN * 2];
    memset(long_str, 'A', sizeof(long_str) - 1);
    long_str[sizeof(long_str) - 1] = '\0';
    
    printf("  Attempting overflow:\n  ");
    safe_string_append(&str, long_str);
    
    printf("  ✅ String operations completed safely\n\n");
}

// ============================================
// PATTERN 6: DEFENSIVE NULL CHECKS
// Always check pointers before use
// ============================================

typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* ✅ GOOD: Defensive null checks */
Node* safe_list_find(Node *head, int value) {
    // Null check first
    if (head == NULL) {
        return NULL;
    }
    
    Node *current = head;
    while (current != NULL) {  // Always check before dereference
        if (current->value == value) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/* ✅ GOOD: Safe list operations */
bool safe_list_insert(Node **head, int value) {
    assert(head != NULL);  // Double pointer must not be null
    
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Failed to allocate node\n");
        return false;
    }
    
    new_node->value = value;
    new_node->next = *head;
    *head = new_node;
    
    return true;
}

void safe_list_destroy(Node **head) {
    if (head == NULL || *head == NULL) {
        return;
    }
    
    Node *current = *head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    
    *head = NULL;  // Prevent use-after-free
}

void null_check_example(void) {
    printf("🛡️  Defensive Null Checks\n");
    
    Node *list = NULL;
    
    safe_list_insert(&list, 10);
    safe_list_insert(&list, 20);
    safe_list_insert(&list, 30);
    
    Node *found = safe_list_find(list, 20);
    if (found != NULL) {
        printf("  Found value: %d\n", found->value);
    }
    
    found = safe_list_find(list, 999);
    if (found == NULL) {
        printf("  Value 999 not found (as expected)\n");
    }
    
    safe_list_destroy(&list);
    printf("  ✅ List destroyed safely\n\n");
}

// ═══════════════════════════════════════════════════════════════════════
// MAIN - Demonstration
// ═══════════════════════════════════════════════════════════════════════

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     🛡️  MEMORY SAFETY PATTERNS IN C                          ║\n");
    printf("║     Guide complet pour éviter malloc et être sûr             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("💡 PHILOSOPHIE: malloc() n'est PAS obligatoire!\n");
    printf("   La plupart des cas peuvent utiliser l'allocation statique.\n\n");
    
    // Patterns statiques (PRÉFÉRER)
    static_allocation_example();
    message_queue_example();
    hash_table_example();
    
    // Patterns avec malloc (si vraiment nécessaire)
    arena_example();
    pool_example();
    cleanup_pattern_example();
    bounds_checking_example();
    safe_string_example();
    null_check_example();
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  🎉 Tous les patterns démontrés!                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("📚 RÉSUMÉ DES RÈGLES:\n\n");
    
    printf("1️⃣  PRÉFÉRER L'ALLOCATION STATIQUE\n");
    printf("   ✅ Tableaux de taille fixe: int data[MAX_SIZE]\n");
    printf("   ✅ Structures sur la stack: Buffer buf;\n");
    printf("   ✅ Variables statiques: static State machine;\n");
    printf("   ❌ Éviter: malloc() pendant l'exécution\n\n");
    
    printf("2️⃣  CONNAÎTRE LES LIMITES MAXIMALES\n");
    printf("   • MAX_USERS, MAX_MESSAGES, MAX_BUFFER_SIZE\n");
    printf("   • Si vous connaissez le max → utilisez un tableau fixe\n");
    printf("   • Documenter ces limites clairement\n\n");
    
    printf("3️⃣  INITIALISATION SYSTÉMATIQUE\n");
    printf("   ✅ Toujours initialiser: Type var = {0};\n");
    printf("   ✅ Utiliser memset() pour structures\n");
    printf("   ✅ Vérifier tous les pointeurs avant usage\n\n");
    
    printf("4️⃣  VÉRIFICATION DES BORNES\n");
    printf("   ✅ Toujours: if (index < size) avant accès\n");
    printf("   ✅ Assertions: assert(ptr != NULL)\n");
    printf("   ✅ Safe strings: strncpy(), strncat()\n\n");
    
    printf("5️⃣  OWNERSHIP CLAIR\n");
    printf("   • Documenter qui possède chaque pointeur\n");
    printf("   • Un seul propriétaire par allocation\n");
    printf("   • Libérer dans l'ordre inverse d'allocation\n\n");
    
    printf("6️⃣  OUTILS DE VALIDATION\n");
    printf("   • AddressSanitizer: -fsanitize=address\n");
    printf("   • Valgrind: valgrind --leak-check=full\n");
    printf("   • Static analyzers: clang --analyze\n");
    printf("   • Warnings stricts: -Wall -Wextra -Werror\n\n");
    
    printf("🔧 COMPILATION RECOMMANDÉE:\n");
    printf("   gcc -Wall -Wextra -Werror -pedantic -std=c11 \\\n");
    printf("       -g -fsanitize=address memory_safety.c\n\n");
    
    printf("💎 AVANTAGES ALLOCATION STATIQUE:\n");
    printf("   ✅ Zéro fuite mémoire possible\n");
    printf("   ✅ Déterministe et prévisible\n");
    printf("   ✅ Performance constante (pas de fragmentation)\n");
    printf("   ✅ Code plus simple et maintenable\n");
    printf("   ✅ Conforme aux standards critiques (NASA, DO-178C)\n\n");
    
    return 0;
}

/*
 * ═══════════════════════════════════════════════════════════════════════
 * PRINCIPES CLÉS POUR LA SÛRETÉ MÉMOIRE
 * ═══════════════════════════════════════════════════════════════════════
 *
 * 🎯 HIÉRARCHIE DES SOLUTIONS (du plus sûr au moins sûr):
 * 
 * 1. ALLOCATION STATIQUE (MEILLEUR) ⭐⭐⭐⭐⭐
 *    ────────────────────────────────
 *    • Tableaux de taille fixe
 *    • Structures sur la stack
 *    • Variables statiques
 *    
 *    Avantages:
 *    ✅ Zéro fuite possible
 *    ✅ Déterministe
 *    ✅ Performance prévisible
 *    ✅ Aucun malloc/free à gérer
 *    
 *    Exemple:
 *    #define MAX_SIZE 100
 *    int data[MAX_SIZE];  // Simple et sûr!
 *
 * 2. OBJECT POOL (SI RÉUTILISATION) ⭐⭐⭐⭐
 *    ────────────────────────────────
 *    • Allocation au démarrage uniquement
 *    • Réutilisation des objets
 *    • Pas de malloc pendant l'exécution
 *    
 *    Exemple:
 *    Connection pool[MAX_CONNECTIONS];
 *    acquire_connection(&pool);  // Réutilise
 *    release_connection(&pool, conn);
 *
 * 3. ARENA ALLOCATOR (SI BESOIN DYNAMIQUE) ⭐⭐⭐
 *    ────────────────────────────────────────
 *    • Une allocation, une libération
 *    • Simplifie la gestion
 *    • Évite les fuites individuelles
 *    
 *    Exemple:
 *    Arena *arena = arena_create(4096);
 *    void *p1 = arena_alloc(arena, 100);
 *    void *p2 = arena_alloc(arena, 200);
 *    arena_destroy(arena);  // Tout libéré!
 *
 * 4. MALLOC CLASSIQUE (DERNIER RECOURS) ⭐⭐
 *    ───────────────────────────────────
 *    • Nécessite gestion rigoureuse
 *    • Risque de fuites
 *    • Utiliser goto cleanup pattern
 *    
 *    ⚠️  À éviter si possible!
 *
 * ═══════════════════════════════════════════════════════════════════════
 * RÈGLES DÉTAILLÉES
 * ═══════════════════════════════════════════════════════════════════════
 *
 * RÈGLE 1: OWNERSHIP CLAIR
 * ────────────────────────
 * • Documenter qui possède chaque pointeur
 * • Un seul propriétaire par allocation
 * • Éviter ownership partagé
 *
 *   /* Takes ownership of buffer */
 *   void process(char *buffer);
 *   
 *   /* Borrows buffer, does not free */
 *   void inspect(const char *buffer);
 *
 * RÈGLE 2: INITIALISATION SYSTÉMATIQUE
 * ─────────────────────────────────────
 * • Initialiser toutes les variables
 * • Préférer calloc() à malloc()
 * • Mettre NULL après free
 *
 *   Buffer buf = {0};  // Structure
 *   int *ptr = calloc(n, sizeof(int));  // Zéro-initialized
 *   free(ptr);
 *   ptr = NULL;  // Évite use-after-free
 *
 * RÈGLE 3: VÉRIFICATION DES BORNES
 * ─────────────────────────────────
 * • Valider TOUS les indices
 * • Vérifier tailles de buffer
 * • Fonctions string sécurisées
 *
 *   if (index < array_size) {  // Toujours vérifier
 *       value = array[index];
 *   }
 *   
 *   strncpy(dst, src, sizeof(dst) - 1);  // Safe
 *   dst[sizeof(dst) - 1] = '\0';  // Force null-term
 *
 * RÈGLE 4: NULL CHECKS PARTOUT
 * ────────────────────────────
 * • Vérifier tous les malloc()
 * • Vérifier avant déréférence
 * • Assertions libérales
 *
 *   void *ptr = malloc(size);
 *   if (ptr == NULL) {  // Toujours vérifier
 *       return ERROR;
 *   }
 *   
 *   assert(ptr != NULL);  // En debug
 *
 * RÈGLE 5: CLEANUP CENTRALISÉ
 * ────────────────────────────
 * • Point de nettoyage unique (goto cleanup)
 * • Libérer dans l'ordre inverse
 * • Évite les fuites sur erreur
 *
 *   int func(void) {
 *       void *p1 = NULL, *p2 = NULL;
 *       
 *       p1 = malloc(100);
 *       if (!p1) goto cleanup;
 *       
 *       p2 = malloc(200);
 *       if (!p2) goto cleanup;
 *       
 *       // Process...
 *       
 *   cleanup:
 *       free(p2);  // Ordre inverse
 *       free(p1);
 *       return result;
 *   }
 *
 * RÈGLE 6: OUTILS DE VALIDATION
 * ──────────────────────────────
 * • AddressSanitizer: -fsanitize=address
 * • Valgrind: --leak-check=full
 * • Static analyzers: clang --analyze
 * • Warnings: -Wall -Wextra -Werror
 *
 * ═══════════════════════════════════════════════════════════════════════
 * VULNÉRABILITÉS PRÉVENUES
 * ═══════════════════════════════════════════════════════════════════════
 *
 * ✅ Buffer overflow           → Bounds checking
 * ✅ Use-after-free            → NULL après free
 * ✅ Double-free               → NULL checks
 * ✅ Memory leaks              → goto cleanup / static alloc
 * ✅ Null pointer dereference  → Assertions + checks
 * ✅ Uninitialized read        → Initialisation systématique
 * ✅ Heap fragmentation        → Static allocation / pools
 * ✅ Non-determinism           → Fixed sizes / no runtime malloc
 *
 * ═══════════════════════════════════════════════════════════════════════
 * QUAND UTILISER QUOI?
 * ═══════════════════════════════════════════════════════════════════════
 *
 * UTILISER ALLOCATION STATIQUE SI:
 * ✅ Taille maximale connue à la compilation
 * ✅ Nombre d'objets borné (MAX_USERS, MAX_CONNECTIONS, etc.)
 * ✅ Durée de vie = durée du programme ou fonction
 * ✅ Système critique (avionique, spatial, médical)
 * ✅ Performance déterministe requise
 *
 * UTILISER OBJECT POOL SI:
 * ✅ Objets réutilisables (connections, buffers, etc.)
 * ✅ Allocation/désallocation fréquente
 * ✅ Taille fixe des objets
 * ✅ Nombre maximal connu
 *
 * UTILISER ARENA SI:
 * ✅ Beaucoup d'allocations courte durée
 * ✅ Libération en bloc acceptable
 * ✅ Pattern "parse → process → discard"
 *
 * UTILISER MALLOC CLASSIQUE SI:
 * ⚠️  Vraiment aucune autre option
 * ⚠️  Taille imprévisible à la compilation
 * ⚠️  Durée de vie complexe et variable
 *
 * ═══════════════════════════════════════════════════════════════════════
 * EXEMPLES DE CONVERSIONS malloc() → static
 * ═══════════════════════════════════════════════════════════════════════
 *
 * ❌ AVANT (avec malloc):
 * ──────────────────────
 * int *data = malloc(100 * sizeof(int));
 * if (data == NULL) return ERROR;
 * // ... use data ...
 * free(data);
 *
 * ✅ APRÈS (statique):
 * ───────────────────
 * int data[100];  // Simple! Aucune fuite possible!
 * // ... use data ...
 * // Pas de free nécessaire!
 *
 * ─────────────────────────────────────────────────────────────────────
 *
 * ❌ AVANT (liste dynamique):
 * ──────────────────────────
 * Node *node = malloc(sizeof(Node));
 * node->next = head;
 * head = node;
 *
 * ✅ APRÈS (pool statique):
 * ────────────────────────
 * Node pool[MAX_NODES];
 * int next_free = 0;
 * pool[next_free++] = new_node;  // Pas de malloc!
 *
 * ═══════════════════════════════════════════════════════════════════════
 *
 * 💡 PHILOSOPHIE: malloc() EST L'EXCEPTION, PAS LA RÈGLE!
 *
 *    La plupart des programmes peuvent fonctionner avec 0 malloc()
 *    pendant leur exécution normale. Questionnez toujours:
 *    "Puis-je utiliser un tableau de taille fixe à la place?"
 *
 * ═══════════════════════════════════════════════════════════════════════
 */
