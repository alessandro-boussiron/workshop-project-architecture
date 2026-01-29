# Règle 7 : Utiliser les Assertions

## 📋 Énoncé

**Utiliser des assertions pour documenter et vérifier les hypothèses**
- Assertions sur les préconditions
- Assertions sur les postconditions
- Assertions sur les invariants
- Assert de défense

## 🎯 Objectif

- **Documentation**: Hypothèses explicites dans le code
- **Détection précoce**: Bugs trouvés immédiatement
- **Vérification**: Contrats de fonction respectés
- **Debugging**: Localisation rapide des problèmes

## ❌ Problèmes

### Sans assertions

```c
// ❌ MAUVAIS - Pas de vérification
void bad_array_access(int *array, size_t size, size_t index) {
    // Que se passe-t-il si:
    // - array est NULL?
    // - index >= size?
    // - size est 0?
    
    array[index] = 42;  // Crash possible!
}

void bad_queue_enqueue(Queue *queue, int value) {
    // Pas de vérification si queue est pleine!
    queue->items[queue->tail] = value;
    queue->tail = (queue->tail + 1) % MAX_SIZE;
    queue->count++;  // Peut dépasser MAX_SIZE!
}
```

## ✅ Solutions

### 1. Assertions sur préconditions

```c
// ✅ BON
#include <assert.h>

void good_array_access(int *array, size_t size, size_t index) {
    // Préconditions
    assert(array != NULL);
    assert(size > 0);
    assert(index < size);
    
    array[index] = 42;
}

void good_queue_enqueue(Queue *queue, int value) {
    // Préconditions
    assert(queue != NULL);
    assert(queue->count < MAX_SIZE);  // Queue pas pleine
    
    queue->items[queue->tail] = value;
    queue->tail = (queue->tail + 1) % MAX_SIZE;
    queue->count++;
    
    // Postcondition
    assert(queue->count <= MAX_SIZE);
}
```

### 2. Assertions sur postconditions

```c
// ✅ BON
int safe_divide(int a, int b, int *result) {
    // Préconditions
    assert(result != NULL);
    assert(b != 0);
    
    *result = a / b;
    
    // Postcondition: résultat raisonnable
    assert(*result * b <= a + abs(b));
    assert(*result * b >= a - abs(b));
    
    return 0;
}
```

### 3. Assertions sur invariants

```c
// ✅ BON - Structure avec invariants
typedef struct {
    int values[10];
    size_t count;  // Invariant: count <= 10
} BoundedArray;

void bounded_array_add(BoundedArray *array, int value) {
    // Préconditions
    assert(array != NULL);
    assert(array->count < 10);  // Invariant
    
    array->values[array->count] = value;
    array->count++;
    
    // Postcondition: invariant maintenu
    assert(array->count <= 10);
}

int bounded_array_get(const BoundedArray *array, size_t index) {
    // Préconditions
    assert(array != NULL);
    assert(array->count <= 10);  // Vérifier invariant
    assert(index < array->count);
    
    return array->values[index];
}
```

## 📊 Types d'Assertions

### 1. Assert sur pointeurs

```c
void process_data(const Data *input, Data *output) {
    // Toujours vérifier les pointeurs
    assert(input != NULL);
    assert(output != NULL);
    
    // Vérifier que pointeurs sont distincts
    assert(input != output);
    
    // Processing...
}
```

### 2. Assert sur indices

```c
void matrix_set(Matrix *m, size_t row, size_t col, int value) {
    assert(m != NULL);
    assert(row < m->rows);
    assert(col < m->cols);
    assert(m->rows <= MAX_MATRIX_SIZE);
    assert(m->cols <= MAX_MATRIX_SIZE);
    
    m->data[row][col] = value;
}
```

### 3. Assert sur plages de valeurs

```c
void set_temperature(Sensor *sensor, float temp) {
    assert(sensor != NULL);
    
    // Température physiquement possible
    assert(temp >= -273.15f);  // Zéro absolu
    assert(temp <= 1000.0f);   // Limite capteur
    
    sensor->temperature = temp;
}
```

### 4. Assert sur état

```c
typedef enum {
    STATE_IDLE,
    STATE_ACTIVE,
    STATE_ERROR
} State;

void process_event(StateMachine *sm, Event event) {
    assert(sm != NULL);
    assert(sm->state == STATE_ACTIVE);  // État valide
    
    // Process event...
    
    // État final valide
    assert(sm->state >= STATE_IDLE && 
           sm->state <= STATE_ERROR);
}
```

## 🔧 Assertions Avancées

### 1. Assert avec message (macro custom)

```c
#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: %s at %s:%d - %s\n", \
                    #condition, __FILE__, __LINE__, message); \
            abort(); \
        } \
    } while(0)

void process(int value) {
    ASSERT_MSG(value >= 0, "Value must be non-negative");
    ASSERT_MSG(value < 100, "Value exceeds maximum");
}
```

### 2. Assert de compilation (static_assert)

```c
#include <assert.h>

// C11 static_assert
_Static_assert(sizeof(int) == 4, "int must be 4 bytes");
_Static_assert(MAX_BUFFER_SIZE > 0, "Buffer size must be positive");

typedef struct {
    uint8_t type;
    uint16_t value;
    uint8_t checksum;
} __attribute__((packed)) Packet;

_Static_assert(sizeof(Packet) == 4, "Packet must be 4 bytes");
```

### 3. Invariant checker functions

```c
// ✅ BON - Fonction de vérification d'invariants
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynamicBuffer;

bool buffer_invariant_holds(const DynamicBuffer *buf) {
    if (buf == NULL) return false;
    if (buf->size > buf->capacity) return false;
    if (buf->capacity > 0 && buf->data == NULL) return false;
    if (buf->capacity == 0 && buf->data != NULL) return false;
    return true;
}

void buffer_append(DynamicBuffer *buf, int value) {
    assert(buffer_invariant_holds(buf));  // Précondition
    assert(buf->size < buf->capacity);
    
    buf->data[buf->size] = value;
    buf->size++;
    
    assert(buffer_invariant_holds(buf));  // Postcondition
}
```

## 📚 Patterns

### 1. Defense in Depth

```c
int critical_operation(const char *data, size_t len) {
    // Layer 1: Assertions (debug)
    assert(data != NULL);
    assert(len > 0);
    assert(len <= MAX_LENGTH);
    
    // Layer 2: Runtime checks (production)
    if (data == NULL || len == 0 || len > MAX_LENGTH) {
        return -1;
    }
    
    // Safe to proceed
    return process(data, len);
}
```

### 2. Assert at boundaries

```c
// ✅ BON - Assertions aux limites
void public_api_function(void *user_data) {
    // Assert at API boundary
    assert(user_data != NULL);
    
    internal_function(user_data);
}

static void internal_function(void *data) {
    // Assert également dans les fonctions internes
    assert(data != NULL);
    
    // Processing...
}
```

### 3. Loop invariants

```c
void binary_search(int *array, size_t size, int target) {
    assert(array != NULL);
    assert(size > 0);
    
    size_t left = 0;
    size_t right = size - 1;
    
    while (left <= right) {
        // Loop invariant
        assert(left < size);
        assert(right < size);
        assert(left <= right);
        
        size_t mid = left + (right - left) / 2;
        
        if (array[mid] == target) {
            return;
        } else if (array[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
}
```

## 🎭 Production vs Debug

### Assertions désactivables

```c
// Assertions actives en DEBUG
#ifdef NDEBUG
    #define ASSERT(x) ((void)0)
#else
    #define ASSERT(x) assert(x)
#endif

// Vérifications runtime toujours actives
#define VERIFY(condition, action) \
    do { \
        if (!(condition)) { \
            action; \
        } \
    } while(0)

void function(int *ptr) {
    // Debug: vérifie avec assertion
    ASSERT(ptr != NULL);
    
    // Production: vérifie et gère l'erreur
    VERIFY(ptr != NULL, return);
    
    // Safe to use ptr
}
```

### Compilation

```bash
# Avec assertions (debug)
gcc -Wall -Wextra -g -DDEBUG program.c

# Sans assertions (production)
gcc -Wall -Wextra -O2 -DNDEBUG program.c
```

## 🔍 Quand utiliser

### ✅ Utiliser assert pour:

- Bugs de programmation (erreurs de logique)
- Vérifier préconditions/postconditions
- Vérifier invariants
- Documenter hypothèses
- Debugging

### ❌ Ne PAS utiliser assert pour:

- Erreurs d'exécution normales
- Validation d'entrées utilisateur
- Erreurs réseau/fichier
- Conditions qui peuvent arriver en production

```c
// ❌ MAUVAIS - Ne pas assert sur input utilisateur
void bad_process_input(const char *user_input) {
    assert(user_input != NULL);  // L'utilisateur peut envoyer NULL!
}

// ✅ BON - Vérifier et gérer
bool good_process_input(const char *user_input) {
    if (user_input == NULL) {
        return false;  // Erreur gérée
    }
    
    // Processing...
    return true;
}

// ✅ BON - Assert sur internal logic
static void internal_helper(const char *data) {
    assert(data != NULL);  // OK, contrôlé par le code
    // Cette fonction est appelée uniquement en interne
}
```

## 🛠️ Outils

### Static Analysis

```bash
# Clang analyzer - detect assert violations
clang --analyze *.c

# Cppcheck
cppcheck --enable=all *.c
```

### Runtime Detection

```bash
# GDB - break on assertion failure
gdb ./program
(gdb) break __assert_fail
(gdb) run
```

## 💡 Points clés

1. **Assert liberally**: Beaucoup d'assertions
2. **Document assumptions**: Hypothèses explicites
3. **Check invariants**: Avant et après opérations
4. **Null checks**: Tous les pointeurs
5. **Range checks**: Toutes les bornes
6. **Disable in production**: Compiler avec -DNDEBUG

## 🎓 Exercices

Voir `exercises/ex07_assertions.c`

---

**Résumé**: Assert sur toutes les préconditions, postconditions et invariants. Documenter les hypothèses.
