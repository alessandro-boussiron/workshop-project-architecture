# Règles de Sûreté Mémoire en C

## 🎯 Philosophie Fondamentale

> **malloc() n'est PAS obligatoire!**
> 
> La majorité des programmes peuvent fonctionner avec **zéro allocation dynamique** pendant leur exécution. L'allocation statique est plus sûre, plus rapide, et plus prévisible.

## 📊 Hiérarchie des Solutions

```
┌─────────────────────────────────────────────────────┐
│ 1. ALLOCATION STATIQUE          ⭐⭐⭐⭐⭐ MEILLEUR  │
│    → Tableaux fixes, stack                          │
│    → Zéro fuite possible                            │
│    → Temps d'exécution constant                     │
└─────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────┐
│ 2. OBJECT POOL                  ⭐⭐⭐⭐ EXCELLENT   │
│    → Allocation au démarrage                        │
│    → Réutilisation des objets                       │
│    → Pas de malloc en runtime                       │
└─────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────┐
│ 3. ARENA ALLOCATOR              ⭐⭐⭐ BON           │
│    → Une allocation, une libération                 │
│    → Simplifie la gestion                           │
│    → Évite fuites individuelles                     │
└─────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────┐
│ 4. MALLOC CLASSIQUE             ⭐⭐ DERNIER RECOURS│
│    → Nécessite gestion rigoureuse                   │
│    → Risques élevés                                 │
│    → À ÉVITER si possible                           │
└─────────────────────────────────────────────────────┘
```

---

## 🔴 RÈGLE 1: Préférer l'Allocation Statique

### Principe

**Si vous connaissez la taille maximale → utilisez un tableau fixe!**

### ❌ Mauvais Exemple (avec malloc)

```c
typedef struct {
    int *data;
    size_t size;
} DynamicArray;

DynamicArray* create_array(size_t size) {
    DynamicArray *arr = malloc(sizeof(DynamicArray));
    if (!arr) return NULL;
    
    arr->data = malloc(size * sizeof(int));
    if (!arr->data) {
        free(arr);
        return NULL;
    }
    
    arr->size = size;
    return arr;
}

void destroy_array(DynamicArray *arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}

// PROBLÈMES:
// 1. Qui appelle destroy_array? Quand?
// 2. Que se passe-t-il si on oublie?
// 3. Double-free possible
// 4. Use-after-free possible
// 5. Fuite mémoire facile
```

### ✅ Bon Exemple (statique)

```c
#define MAX_ARRAY_SIZE 1000

typedef struct {
    int data[MAX_ARRAY_SIZE];  // ✅ Statique!
    size_t count;               // Nombre d'éléments utilisés
} StaticArray;

void init_array(StaticArray *arr) {
    memset(arr, 0, sizeof(StaticArray));
}

bool array_add(StaticArray *arr, int value) {
    if (arr->count >= MAX_ARRAY_SIZE) {
        return false;  // Full
    }
    
    arr->data[arr->count++] = value;
    return true;
}

// AVANTAGES:
// ✅ Aucune fuite possible
// ✅ Pas de malloc/free à gérer
// ✅ Performance prévisible
// ✅ Code plus simple
// ✅ Déterministe (systèmes critiques)
```

### 📏 Exemples de Limites Raisonnables

```c
// Systèmes embarqués / critiques
#define MAX_USERS           100
#define MAX_CONNECTIONS     50
#define MAX_MESSAGES        256
#define MAX_BUFFER_SIZE     4096
#define MAX_QUEUE_SIZE      128
#define MAX_FILENAME_LEN    256
#define MAX_PATH_LEN        1024

// Ces limites sont:
// ✅ Documentées
// ✅ Validées (retour false si dépassé)
// ✅ Suffisantes pour 99% des cas
// ✅ Prévisibles en mémoire
```

---

## 🟠 RÈGLE 2: Structures de Données Sans malloc

### File Circulaire (Ring Buffer)

```c
#define RING_SIZE 64

typedef struct {
    int data[RING_SIZE];  // ✅ Fixe
    size_t head;
    size_t tail;
    size_t count;
} RingBuffer;

void ring_init(RingBuffer *ring) {
    memset(ring, 0, sizeof(RingBuffer));
}

bool ring_push(RingBuffer *ring, int value) {
    if (ring->count >= RING_SIZE) {
        return false;  // Full
    }
    
    ring->data[ring->tail] = value;
    ring->tail = (ring->tail + 1) % RING_SIZE;
    ring->count++;
    return true;
}

bool ring_pop(RingBuffer *ring, int *value) {
    if (ring->count == 0) {
        return false;  // Empty
    }
    
    *value = ring->data[ring->head];
    ring->head = (ring->head + 1) % RING_SIZE;
    ring->count--;
    return true;
}

// ✅ Aucun malloc nécessaire!
```

### Stack Statique

```c
#define STACK_SIZE 128

typedef struct {
    int data[STACK_SIZE];
    size_t top;
} Stack;

void stack_init(Stack *s) {
    s->top = 0;
}

bool stack_push(Stack *s, int value) {
    if (s->top >= STACK_SIZE) {
        return false;
    }
    s->data[s->top++] = value;
    return true;
}

bool stack_pop(Stack *s, int *value) {
    if (s->top == 0) {
        return false;
    }
    *value = s->data[--s->top];
    return true;
}
```

### Liste Chaînée avec Indices

```c
#define MAX_NODES 100

typedef struct {
    int data;
    int next;  // Index, pas pointeur! (-1 = NULL)
} Node;

typedef struct {
    Node nodes[MAX_NODES];
    int head;
    int free_list;
    size_t count;
} IndexedList;

void list_init(IndexedList *list) {
    list->head = -1;
    list->count = 0;
    
    // Init free list
    for (int i = 0; i < MAX_NODES - 1; i++) {
        list->nodes[i].next = i + 1;
    }
    list->nodes[MAX_NODES - 1].next = -1;
    list->free_list = 0;
}

bool list_insert(IndexedList *list, int value) {
    if (list->free_list == -1) {
        return false;  // No free nodes
    }
    
    // Take from free list
    int new_idx = list->free_list;
    list->free_list = list->nodes[new_idx].next;
    
    // Insert at head
    list->nodes[new_idx].data = value;
    list->nodes[new_idx].next = list->head;
    list->head = new_idx;
    list->count++;
    
    return true;
}

// ✅ Gestion mémoire manuelle mais statique!
```

---

## 🟡 RÈGLE 3: Object Pool Pattern

### Quand Utiliser?

- ✅ Objets **réutilisables** (connections, buffers, workers)
- ✅ **Nombre maximum connu** à l'avance
- ✅ **Allocation/libération fréquente**
- ✅ Besoin de **performances prévisibles**

### Exemple: Connection Pool

```c
#define MAX_CONNECTIONS 32

typedef struct {
    int socket_fd;
    bool is_active;
    char remote_addr[64];
} Connection;

typedef struct {
    Connection pool[MAX_CONNECTIONS];
    size_t active_count;
} ConnectionPool;

void pool_init(ConnectionPool *pool) {
    memset(pool, 0, sizeof(ConnectionPool));
    
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        pool->pool[i].socket_fd = -1;
        pool->pool[i].is_active = false;
    }
}

Connection* pool_acquire(ConnectionPool *pool) {
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        if (!pool->pool[i].is_active) {
            pool->pool[i].is_active = true;
            pool->active_count++;
            return &pool->pool[i];
        }
    }
    
    fprintf(stderr, "Connection pool exhausted!\n");
    return NULL;
}

void pool_release(ConnectionPool *pool, Connection *conn) {
    if (!conn || !conn->is_active) {
        return;
    }
    
    // Cleanup connection
    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
        conn->socket_fd = -1;
    }
    
    conn->is_active = false;
    pool->active_count--;
}

// Usage
void example_usage(void) {
    ConnectionPool pool;
    pool_init(&pool);
    
    // Acquire
    Connection *conn = pool_acquire(&pool);
    if (conn) {
        conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        strcpy(conn->remote_addr, "192.168.1.100");
        
        // Use connection...
        
        // Release when done
        pool_release(&pool, conn);
    }
    
    // ✅ Aucune fuite possible!
}
```

---

## 🟢 RÈGLE 4: Vérification des Bornes TOUJOURS

### String Safety

```c
// ❌ DANGEREUX
void bad_string_copy(char *dest, const char *src) {
    strcpy(dest, src);  // Buffer overflow possible!
    strcat(dest, ".txt");  // Buffer overflow possible!
}

// ✅ SÛR
void safe_string_copy(char *dest, size_t dest_size, const char *src) {
    // Vérifier src
    if (!src) {
        dest[0] = '\0';
        return;
    }
    
    // Copy avec limite
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';  // Force null termination
    
    // Append avec limite
    size_t current_len = strlen(dest);
    size_t remaining = dest_size - current_len - 1;
    
    if (remaining > 0) {
        strncat(dest, ".txt", remaining);
    }
}
```

### Array Access Safety

```c
// ❌ DANGEREUX
int get_value(int *array, int index) {
    return array[index];  // Pas de vérification!
}

// ✅ SÛR
bool get_value_safe(const int *array, size_t size, size_t index, int *out) {
    if (!array || !out) {
        return false;
    }
    
    if (index >= size) {
        fprintf(stderr, "Index %zu out of bounds (size %zu)\n", index, size);
        return false;
    }
    
    *out = array[index];
    return true;
}
```

---

## 🔵 RÈGLE 5: Initialisation Systématique

### Structures

```c
// ✅ Méthode 1: Designated initializers
typedef struct {
    int x;
    int y;
    int z;
} Point;

Point p = {0};  // Tout à zéro
Point p2 = {.x = 1, .y = 2, .z = 3};  // Explicite

// ✅ Méthode 2: memset
Point p3;
memset(&p3, 0, sizeof(Point));

// ✅ Méthode 3: Fonction d'init
void point_init(Point *p) {
    assert(p != NULL);
    p->x = 0;
    p->y = 0;
    p->z = 0;
}
```

### Tableaux

```c
// ✅ Initialisation complète
int numbers[100] = {0};  // Tous à zéro

// ✅ Initialisation partielle (reste à zéro)
int values[10] = {1, 2, 3};  // {1,2,3,0,0,0,0,0,0,0}

// ✅ Avec memset
char buffer[256];
memset(buffer, 0, sizeof(buffer));
```

### Pointeurs

```c
// ✅ Toujours initialiser
int *ptr = NULL;  // Pas de garbage

// ✅ Après free
free(ptr);
ptr = NULL;  // Évite use-after-free

// ✅ Vérifier avant usage
if (ptr != NULL) {
    *ptr = 42;
}
```

---

## 🟣 RÈGLE 6: Pattern de Cleanup (goto cleanup)

### Le Problème

```c
// ❌ MAUVAIS - Fuites sur erreur
int bad_process(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;
    
    char *buffer = malloc(1024);
    if (!buffer) return -1;  // FUITE: file pas fermé!
    
    int *data = malloc(sizeof(int) * 100);
    if (!data) return -1;  // FUITE: file + buffer!
    
    // Process...
    
    free(data);
    free(buffer);
    fclose(file);
    return 0;
}
```

### La Solution

```c
// ✅ BON - goto cleanup pattern
int good_process(const char *filename) {
    int result = -1;
    FILE *file = NULL;
    char *buffer = NULL;
    int *data = NULL;
    
    // Phase d'acquisition
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Cannot open file\n");
        goto cleanup;
    }
    
    buffer = malloc(1024);
    if (!buffer) {
        fprintf(stderr, "Cannot allocate buffer\n");
        goto cleanup;
    }
    
    data = malloc(sizeof(int) * 100);
    if (!data) {
        fprintf(stderr, "Cannot allocate data\n");
        goto cleanup;
    }
    
    // Phase de traitement
    size_t bytes = fread(buffer, 1, 1024, file);
    if (bytes > 0) {
        // Process data...
        result = 0;  // Success
    }
    
cleanup:
    // Phase de nettoyage - TOUJOURS exécutée
    free(data);      // Safe même si NULL
    free(buffer);    // Safe même si NULL
    if (file) {
        fclose(file);
    }
    
    return result;
}

// ✅ Garanties:
// • Toutes les ressources sont libérées
// • Même en cas d'erreur
// • Un seul point de sortie
// • Code facile à maintenir
```

---

## ⚫ RÈGLE 7: Outils de Validation

### AddressSanitizer

```bash
# Compilation
gcc -Wall -Wextra -Werror -g -fsanitize=address program.c

# Détecte:
# • Buffer overflow
# • Use-after-free
# • Use-after-return
# • Double-free
# • Memory leaks
```

### Valgrind

```bash
# Analyse complète
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./program

# Détecte:
# • Memory leaks
# • Invalid reads/writes
# • Uninitialized values
```

### Static Analysis

```bash
# Clang analyzer
clang --analyze -Xanalyzer -analyzer-output=text *.c

# Cppcheck
cppcheck --enable=all --suppress=missingIncludeSystem *.c

# Détectent:
# • Null pointer dereferences
# • Resource leaks
# • Logic errors
# • Dead code
```

---

## 📋 Checklist de Code Review

### Allocation Mémoire

- [ ] Utilise l'allocation statique quand possible?
- [ ] Taille maximale documentée (`MAX_*` defines)?
- [ ] Évite malloc() pendant l'exécution?
- [ ] Utilise Object Pool pour réutilisation?
- [ ] Pattern goto cleanup pour ressources?

### Initialisation

- [ ] Toutes les variables initialisées?
- [ ] Structures avec `= {0}` ou `memset`?
- [ ] Pointeurs à NULL par défaut?
- [ ] NULL après chaque free?

### Vérifications

- [ ] Bornes vérifiées avant accès tableau?
- [ ] NULL checks avant déréférence?
- [ ] Retours de malloc() vérifiés?
- [ ] Assertions sur préconditions?
- [ ] `strncpy` au lieu de `strcpy`?

### Libération

- [ ] Chaque malloc a un free correspondant?
- [ ] Libération dans l'ordre inverse?
- [ ] Cleanup sur tous les chemins d'erreur?
- [ ] Pas de use-after-free possible?
- [ ] Pas de double-free possible?

### Outils

- [ ] Compile avec `-Wall -Wextra -Werror`?
- [ ] Testé avec AddressSanitizer?
- [ ] Vérifié avec Valgrind?
- [ ] Analyse statique passée?

---

## 🎓 Exercices Pratiques

### Exercice 1: Conversion malloc → static

Convertir ce code pour utiliser l'allocation statique:

```c
// ❌ AVANT
typedef struct {
    char *name;
    int *scores;
    size_t score_count;
} Player;

Player* create_player(const char *name, size_t max_scores) {
    Player *p = malloc(sizeof(Player));
    if (!p) return NULL;
    
    p->name = malloc(strlen(name) + 1);
    if (!p->name) {
        free(p);
        return NULL;
    }
    strcpy(p->name, name);
    
    p->scores = malloc(max_scores * sizeof(int));
    if (!p->scores) {
        free(p->name);
        free(p);
        return NULL;
    }
    
    p->score_count = 0;
    return p;
}
```

Solution dans `exercises/memory_safety_exercises.c`

### Exercice 2: Ring Buffer

Implémenter un ring buffer de strings sans malloc:
- Capacité: 32 messages
- Taille max message: 128 chars
- Opérations: push, pop, is_full, is_empty

### Exercice 3: Object Pool

Créer un pool de 16 workers threads:
- Acquisition/libération
- Statistiques (peak usage)
- Thread-safe avec mutex

---

## 📚 Ressources

### Standards et Guidelines

- NASA "Power of 10" Rules
- MISRA C Guidelines
- DO-178C (Avionique)
- IEC 61508 (Sécurité fonctionnelle)

### Outils

- [AddressSanitizer](https://github.com/google/sanitizers)
- [Valgrind](https://valgrind.org/)
- [Clang Static Analyzer](https://clang-analyzer.llvm.org/)
- [Cppcheck](http://cppcheck.sourceforge.net/)

### Lectures

- "Secure Coding in C and C++" - Robert Seacord
- "The Art of Software Security Assessment" - Dowd, McDonald, Schuh
- NASA JPL Coding Standards

---

## 💡 Résumé en Une Page

```
╔══════════════════════════════════════════════════════════════╗
║              RÈGLES DE SÛRETÉ MÉMOIRE EN C                   ║
╚══════════════════════════════════════════════════════════════╝

1. PRÉFÉRER STATIQUE
   int data[MAX_SIZE];  ← Simple, sûr, rapide

2. LIMITES DOCUMENTÉES
   #define MAX_USERS 100  ← Connu et validé

3. OBJECT POOL SI RÉUTILISATION
   Connection pool[MAX_CONNS];  ← Allocation au démarrage

4. BORNES VÉRIFIÉES
   if (index < size) { ... }  ← Toujours

5. INITIALISATION SYSTÉMATIQUE
   Buffer buf = {0};  ← Pas de garbage

6. CLEANUP CENTRALISÉ
   goto cleanup;  ← Un seul point de sortie

7. OUTILS DE VALIDATION
   -fsanitize=address  ← Détecter problèmes tôt

╔══════════════════════════════════════════════════════════════╗
║ AVANTAGES ALLOCATION STATIQUE:                              ║
║ ✅ Zéro fuite possible                                       ║
║ ✅ Performance déterministe                                  ║
║ ✅ Code plus simple                                          ║
║ ✅ Conforme standards critiques                              ║
╚══════════════════════════════════════════════════════════════╝

💡 malloc() EST L'EXCEPTION, PAS LA RÈGLE!
```
