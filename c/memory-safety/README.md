# Memory Safety en C - Sans malloc!

## 🎯 Philosophie

> **malloc() n'est PAS obligatoire!**
> 
> 90% des programmes peuvent fonctionner avec **zéro allocation dynamique** pendant leur exécution.

## 📚 Documentation

### À Lire en Premier
1. **[MEMORY_RULES.md](MEMORY_RULES.md)** - Guide complet des règles de sûreté mémoire
   - Pourquoi éviter malloc()
   - Hiérarchie des solutions (static > pools > arena > malloc)
   - 7 règles fondamentales avec exemples détaillés
   - Checklist de code review

2. **[memory_safety.c](memory_safety.c)** - Code démonstration
   - Patterns Arena Allocator et Object Pool
   - Cleanup pattern (goto cleanup)
   - Bounds checking
   - Safe string operations

### Compléments NASA
- `../nasa-rules/docs/RULE03.md` - No dynamic memory (règle NASA)
- `../nasa-rules/exercises/ex03_static_memory.c` - Exercice pratique

## ⭐ Points Clés

### 1. Allocation Statique (PRÉFÉRER)
```c
#define MAX_SIZE 1000

typedef struct {
    int data[MAX_SIZE];  // ✅ Statique!
    size_t count;
} Buffer;

Buffer buf = {0};  // Simple, sûr, rapide
// Pas de malloc, pas de free, pas de fuite possible!
```

### 2. Taille Max Connue = Tableau Fixe
```c
// Si vous connaissez le max → utilisez un tableau statique
#define MAX_USERS 100
#define MAX_CONNECTIONS 50
#define MAX_MESSAGES 256

User users[MAX_USERS];        // ✅ Prévisible
Connection pool[MAX_CONNECTIONS];  // ✅ Déterministe
Message queue[MAX_MESSAGES];  // ✅ Sans malloc
```

### 3. Structures Complètes Sans malloc
```c
// File circulaire (ring buffer)
typedef struct {
    int data[64];
    size_t head, tail, count;
} RingBuffer;

// Hash table statique
typedef struct {
    Entry entries[128];
    size_t count;
} HashTable;

// Liste avec indices (pas de pointeurs)
typedef struct {
    Node nodes[100];
    int head, free_list;
} IndexedList;
```

## 🔥 Avantages Allocation Statique

- ✅ **Zéro fuite** mémoire possible
- ✅ **Déterministe** - temps d'exécution prévisible
- ✅ **Performance** constante (pas de fragmentation)
- ✅ **Simplicité** - code plus facile à maintenir
- ✅ **Conforme** aux standards critiques (NASA, DO-178C, MISRA-C)

## 📊 Hiérarchie des Solutions

```
1. ALLOCATION STATIQUE      ⭐⭐⭐⭐⭐ (PRÉFÉRER)
   └─> Tableaux fixes, stack
   └─> Zéro fuite, déterministe

2. OBJECT POOL              ⭐⭐⭐⭐ (Si réutilisation)
   └─> Allocation au démarrage
   └─> Pas de malloc en runtime

3. ARENA ALLOCATOR          ⭐⭐⭐ (Si vraiment nécessaire)
   └─> Une allocation, une libération
   └─> Simplifie la gestion

4. MALLOC CLASSIQUE         ⭐⭐ (DERNIER RECOURS)
   └─> Risques élevés
   └─> À ÉVITER si possible
```

## 🛠️ Compilation et Validation

### Compilation Stricte
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c11 \
    -g -fsanitize=address memory_safety.c -o memory_safety
```

### Exécution
```bash
./memory_safety
```

### Validation Complète
```bash
# AddressSanitizer (déjà inclus ci-dessus)
# Détecte: buffer overflow, use-after-free, memory leaks

# Valgrind
valgrind --leak-check=full ./memory_safety

# Static Analysis
clang --analyze memory_safety.c
cppcheck --enable=all memory_safety.c
```

## 📋 Règles Rapides

### 7 Règles de Sûreté Mémoire

1. **PRÉFÉRER STATIQUE**: Tableau fixe au lieu de malloc
2. **LIMITES DOCUMENTÉES**: #define MAX_SIZE, vérifié
3. **INITIALISATION**: Toujours `= {0}` ou `memset()`
4. **BORNES VÉRIFIÉES**: `if (index < size)` systématique
5. **OWNERSHIP CLAIR**: Qui possède? Qui libère?
6. **CLEANUP CENTRALISÉ**: goto cleanup pattern
7. **OUTILS**: -fsanitize=address, Valgrind, clang --analyze

### Exemples Concrets

#### ❌ MAUVAIS (avec malloc)
```c
char *buffer = malloc(100);
if (!buffer) return -1;
// ... risque de fuite ...
free(buffer);
```

#### ✅ BON (statique)
```c
char buffer[100] = {0};
// ... aucune fuite possible ...
// Pas de free nécessaire!
```

---

#### ❌ MAUVAIS (allocation dynamique)
```c
int *data = malloc(n * sizeof(int));
if (!data) return -1;
// Qui libère? Quand? Comment?
```

#### ✅ BON (taille maximale)
```c
#define MAX_DATA 1000
int data[MAX_DATA];
size_t count = min(n, MAX_DATA);
// Simple et sûr!
```

## 🎓 Exercices

1. Convertir code avec malloc → allocation statique
2. Implémenter ring buffer sans malloc
3. Créer object pool thread-safe
4. Hash table statique
5. File de messages pour système embarqué

Voir `MEMORY_RULES.md` pour exercices détaillés.

## 📖 Ressources

- **NASA Power of 10 Rules** - Règle 3: No dynamic memory after initialization
- **MISRA-C** - Guidelines pour systèmes critiques
- **DO-178C** - Standard avionique
- **IEC 61508** - Sécurité fonctionnelle

## 💡 Résumé

> **malloc() est l'EXCEPTION, pas la RÈGLE!**
> 
> La plupart des cas peuvent utiliser l'allocation statique.
> Résultat: code plus sûr, plus simple, plus prévisible.

---

**Pour une compréhension complète, lire [MEMORY_RULES.md](MEMORY_RULES.md) en premier!**
