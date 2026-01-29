# Règle 8 : Limiter l'Indirection des Pointeurs

## 📋 Énoncé

**Limiter l'indirection des pointeurs à 2 niveaux maximum**
- Pas de triple pointeurs (***ptr)
- Simplifier les structures de données
- Utiliser des indices plutôt que des pointeurs multiples
- Améliorer la lisibilité

## 🎯 Objectif

- **Lisibilité**: Code plus facile à comprendre
- **Maintenance**: Moins d'erreurs de pointeurs
- **Performance**: Meilleur cache locality
- **Sûreté**: Moins de confusion et de bugs

## ❌ Problèmes

### Trop d'indirection

```c
// ❌ MAUVAIS - Triple indirection
void bad_3d_array(int ***array, size_t x, size_t y, size_t z) {
    // Très difficile à lire et maintenir
    int value = array[x][y][z];
    ***array = 42;  // Confus!
}

// ❌ MAUVAIS - Fonction retournant un triple pointeur
char ***bad_get_word_list(void) {
    // Trop complexe!
    static char ***words = NULL;
    return words;
}

// ❌ MAUVAIS - Pointeur vers pointeur de fonction
typedef void (*Handler)(int);
void bad_register_handler(Handler **handler_ptr) {
    // Indirection excessive
    **handler_ptr = my_function;
}
```

## ✅ Solutions

### 1. Tableaux 3D → Tableaux plats

```c
// ✅ BON - Tableau 1D avec indexation calculée
typedef struct {
    int *data;          // 1 niveau d'indirection
    size_t dim_x;
    size_t dim_y;
    size_t dim_z;
} Matrix3D;

// Fonction d'indexation
static inline size_t index_3d(const Matrix3D *m, 
                              size_t x, size_t y, size_t z) {
    assert(m != NULL);
    assert(x < m->dim_x);
    assert(y < m->dim_y);
    assert(z < m->dim_z);
    
    return (x * m->dim_y * m->dim_z) + (y * m->dim_z) + z;
}

int matrix3d_get(const Matrix3D *m, size_t x, size_t y, size_t z) {
    size_t idx = index_3d(m, x, y, z);
    return m->data[idx];  // Simple!
}

void matrix3d_set(Matrix3D *m, size_t x, size_t y, size_t z, int value) {
    size_t idx = index_3d(m, x, y, z);
    m->data[idx] = value;
}
```

### 2. Liste chainée avec indices

```c
// ❌ MAUVAIS - Pointeur vers pointeur de noeud
typedef struct Node {
    int data;
    struct Node *next;
} Node;

void bad_insert(Node **head_ptr, int value) {
    // 2 niveaux OK, mais peut mieux faire
}

// ✅ BON - Liste avec array + indices
#define MAX_NODES 100

typedef struct {
    int data;
    int next_index;  // -1 = null
} IndexNode;

typedef struct {
    IndexNode nodes[MAX_NODES];
    int head_index;
    int free_index;
    size_t count;
} IndexList;

void index_list_insert(IndexList *list, int value) {
    assert(list != NULL);
    assert(list->count < MAX_NODES);
    
    // Prendre un noeud libre
    int new_idx = list->free_index;
    assert(new_idx >= 0 && new_idx < MAX_NODES);
    
    list->free_index = list->nodes[new_idx].next_index;
    
    // Insérer en tête
    list->nodes[new_idx].data = value;
    list->nodes[new_idx].next_index = list->head_index;
    list->head_index = new_idx;
    list->count++;
}
```

### 3. Arbre avec array

```c
// ✅ BON - Arbre binaire sans pointeurs multiples
#define MAX_TREE_NODES 256

typedef struct {
    int value;
    int left_index;   // -1 = null
    int right_index;  // -1 = null
} TreeNode;

typedef struct {
    TreeNode nodes[MAX_TREE_NODES];
    int root_index;
    size_t count;
} BinaryTree;

void tree_insert(BinaryTree *tree, int value) {
    assert(tree != NULL);
    assert(tree->count < MAX_TREE_NODES);
    
    int new_idx = (int)tree->count;
    tree->nodes[new_idx].value = value;
    tree->nodes[new_idx].left_index = -1;
    tree->nodes[new_idx].right_index = -1;
    
    if (tree->root_index == -1) {
        tree->root_index = new_idx;
    } else {
        // Trouver la position (max 2 niveaux d'indirection)
        int current = tree->root_index;
        while (current != -1) {
            TreeNode *node = &tree->nodes[current];  // 1 niveau
            
            if (value < node->value) {
                if (node->left_index == -1) {
                    node->left_index = new_idx;
                    break;
                }
                current = node->left_index;
            } else {
                if (node->right_index == -1) {
                    node->right_index = new_idx;
                    break;
                }
                current = node->right_index;
            }
        }
    }
    
    tree->count++;
}
```

## 📐 Patterns de Remplacement

### 1. String array (char **) → Structure

```c
// ❌ Acceptable mais limité - 2 niveaux
void process_args(char **argv, int argc) {
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);  // OK
    }
}

// ✅ MIEUX - Structure avec metadata
#define MAX_ARGS 32
#define MAX_ARG_LEN 256

typedef struct {
    char args[MAX_ARGS][MAX_ARG_LEN];
    size_t count;
} ArgumentList;

void process_arguments(const ArgumentList *args) {
    assert(args != NULL);
    assert(args->count <= MAX_ARGS);
    
    for (size_t i = 0; i < args->count; i++) {
        printf("%s\n", args->args[i]);  // Plus simple!
    }
}
```

### 2. Tableaux de pointeurs de fonctions

```c
// ✅ BON - Max 2 niveaux
typedef void (*EventHandler)(int event_id);

typedef struct {
    EventHandler handlers[MAX_EVENTS];  // 1 niveau
    size_t count;
} HandlerRegistry;

void registry_call(const HandlerRegistry *reg, size_t idx, int event_id) {
    assert(reg != NULL);
    assert(idx < reg->count);
    assert(reg->handlers[idx] != NULL);
    
    EventHandler handler = reg->handlers[idx];  // 1 niveau
    handler(event_id);  // Simple!
}
```

### 3. Cache-friendly flat arrays

```c
// ✅ BON - Structure plate pour performance
typedef struct {
    // Au lieu de: Position **grid (2 niveaux)
    // Utiliser tableau plat:
    int grid[GRID_HEIGHT][GRID_WIDTH];
} GameGrid;

void grid_update(GameGrid *g, size_t x, size_t y, int value) {
    assert(g != NULL);
    assert(x < GRID_HEIGHT);
    assert(y < GRID_WIDTH);
    
    g->grid[x][y] = value;  // Simple et cache-friendly
}
```

## 🧮 Calcul des niveaux

```c
int value;           // 0 niveaux
int *ptr;            // 1 niveau  ✅
int **ptr_ptr;       // 2 niveaux ✅ (limite)
int ***ptr_ptr_ptr;  // 3 niveaux ❌ (interdit!)

// Exemples contextuels
int array[10];                    // 0 niveaux
int *array_ptr = array;          // 1 niveau  ✅

char *strings[5];                 // 1 niveau  ✅
char **string_ptr = strings;      // 2 niveaux ✅

// Fonction prenant double pointeur
void modify_pointer(int **ptr_to_ptr) {  // 2 niveaux ✅
    *ptr_to_ptr = NULL;  // OK
}
```

## 🏗️ Architecture

### Design Pattern: Handles/Indices

```c
// ✅ EXCELLENT - System avec handles
#define MAX_RESOURCES 64

typedef int ResourceHandle;  // Simple entier!

typedef struct {
    bool in_use;
    char name[32];
    int data;
} Resource;

typedef struct {
    Resource resources[MAX_RESOURCES];
} ResourceManager;

ResourceHandle resource_allocate(ResourceManager *mgr, const char *name) {
    assert(mgr != NULL);
    assert(name != NULL);
    
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (!mgr->resources[i].in_use) {
            mgr->resources[i].in_use = true;
            strncpy(mgr->resources[i].name, name, 31);
            return i;  // Handle = index
        }
    }
    return -1;  // Erreur
}

Resource *resource_get(ResourceManager *mgr, ResourceHandle handle) {
    assert(mgr != NULL);
    assert(handle >= 0 && handle < MAX_RESOURCES);
    assert(mgr->resources[handle].in_use);
    
    return &mgr->resources[handle];  // 1 niveau seulement!
}
```

## 🔍 Vérification

### Comptage d'indirection

```bash
# Rechercher triple pointeurs
grep -n '\*\*\*' *.c

# Rechercher quadruple pointeurs (!!!)
grep -n '\*\*\*\*' *.c
```

### Review checklist

- [ ] Aucun triple pointeur (***) dans le code
- [ ] Pointeurs de fonction max 2 niveaux
- [ ] Tableaux multidimensionnels justifiés
- [ ] Alternatives avec indices considérées
- [ ] Structures plates préférées

## 💡 Points clés

1. **Maximum 2 étoiles**: `int **ptr` OK, `int ***ptr` interdit
2. **Préférer indices**: Plus simple que pointeurs multiples
3. **Flat arrays**: Meilleure performance cache
4. **Handles**: Abstraction sûre et simple
5. **Structures**: Encapsuler la complexité

## ⚠️ Pièges à éviter

```c
// ❌ PIÈGE 1 - Triple pointeur caché
typedef char **StringArray;
void bad_function(StringArray *arr) {
    // arr est char ***, interdit!
}

// ✅ SOLUTION - Restructurer
typedef struct {
    char strings[MAX_STRINGS][MAX_STRING_LEN];
    size_t count;
} StringList;

void good_function(StringList *list) {
    // list est StringList *, OK!
}

// ❌ PIÈGE 2 - argv implicite
int main(int argc, char ***argv) {  // ❌ Interdit!
    // ...
}

// ✅ CORRECT - Signature standard
int main(int argc, char **argv) {  // ✅ OK (2 niveaux)
    // argv est char **, acceptable
}
```

## 🎓 Exercices

Voir `exercises/ex08_pointer_indirection.c`

---

**Résumé**: Maximum 2 niveaux d'indirection. Utiliser indices et structures plates plutôt que pointeurs multiples.
