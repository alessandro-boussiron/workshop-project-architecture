# Règle 1 : Limiter le Flux de Contrôle

## 📋 Énoncé

**Éviter les constructions de contrôle complexes**
- ❌ Pas de `goto`
- ❌ Pas de `setjmp` / `longjmp`
- ❌ Pas de récursion (directe ou indirecte)

## 🎯 Objectif

Garantir un flux de contrôle **prédictible** et **vérifiable** statiquement.

## ❌ Pourquoi c'est interdit

### 1. goto - Spaghetti Code

```c
// ❌ MAUVAIS
int process_data(int *data, size_t size) {
    int i = 0;
    
start:
    if (i >= size) goto end;
    
    if (data[i] < 0) goto error;
    
    data[i] *= 2;
    i++;
    goto start;
    
error:
    return -1;
    
end:
    return 0;
}
```

**Problèmes:**
- Flux illisible
- Difficile à maintenir
- Impossible à analyser statiquement
- Bugs cachés

### 2. setjmp/longjmp - Exceptions dangereuses

```c
// ❌ MAUVAIS
#include <setjmp.h>

jmp_buf error_buf;

void dangerous_function(void) {
    FILE *f = fopen("data.txt", "r");
    
    if (some_error) {
        longjmp(error_buf, 1);  // Fuite mémoire! f jamais fermé
    }
    
    fclose(f);
}

int main(void) {
    if (setjmp(error_buf) == 0) {
        dangerous_function();
    } else {
        printf("Error occurred\n");
    }
}
```

**Problèmes:**
- État de la pile corrompu
- Fuites de ressources
- Variables locales perdues
- Non-portable

### 3. Récursion - Stack overflow

```c
// ❌ MAUVAIS - Récursion directe
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  // Stack overflow si n grand
}

// ❌ MAUVAIS - Récursion indirecte
void function_a(int n);
void function_b(int n);

void function_a(int n) {
    if (n > 0) function_b(n - 1);
}

void function_b(int n) {
    if (n > 0) function_a(n - 1);  // Récursion cachée!
}
```

**Problèmes:**
- Consommation mémoire imprévisible
- Stack overflow possible
- Difficile à tester
- Performances variables

## ✅ Solutions

### 1. Remplacer goto par contrôle structuré

```c
// ✅ BON - Early return
int process_data(int *data, size_t size) {
    if (data == NULL) {
        return -1;
    }
    
    for (size_t i = 0; i < size; i++) {
        if (data[i] < 0) {
            return -1;  // Early return au lieu de goto error
        }
        data[i] *= 2;
    }
    
    return 0;
}
```

### 2. Gestion d'erreur avec cleanup pattern

```c
// ✅ BON - Cleanup structuré
int safe_file_processing(const char *filename) {
    FILE *file = NULL;
    char *buffer = NULL;
    int result = -1;
    
    file = fopen(filename, "r");
    if (file == NULL) {
        goto cleanup;  // Exception: cleanup acceptable
    }
    
    buffer = malloc(1024);
    if (buffer == NULL) {
        goto cleanup;
    }
    
    // Traitement...
    result = 0;
    
cleanup:
    free(buffer);
    if (file != NULL) {
        fclose(file);
    }
    return result;
}
```

**Note**: `goto` pour cleanup centralisé est une exception acceptée dans certains projets (Linux kernel).

### 3. Machine à états au lieu de goto

```c
// ✅ BON - State machine
typedef enum {
    STATE_INIT,
    STATE_PROCESSING,
    STATE_DONE,
    STATE_ERROR
} State;

int process_with_states(int *data, size_t size) {
    State state = STATE_INIT;
    size_t index = 0;
    
    while (state != STATE_DONE && state != STATE_ERROR) {
        switch (state) {
            case STATE_INIT:
                if (data == NULL || size == 0) {
                    state = STATE_ERROR;
                } else {
                    state = STATE_PROCESSING;
                }
                break;
                
            case STATE_PROCESSING:
                if (index >= size) {
                    state = STATE_DONE;
                } else if (data[index] < 0) {
                    state = STATE_ERROR;
                } else {
                    data[index] *= 2;
                    index++;
                }
                break;
                
            default:
                state = STATE_ERROR;
                break;
        }
    }
    
    return (state == STATE_DONE) ? 0 : -1;
}
```

### 4. Récursion → Itération

```c
// ✅ BON - Itératif
int factorial_iterative(int n) {
    if (n < 0 || n > 20) {
        return -1;  // Éviter overflow
    }
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// ✅ BON - Parcours d'arbre itératif avec stack explicite
#define MAX_STACK_SIZE 100

typedef struct {
    TreeNode *nodes[MAX_STACK_SIZE];
    int top;
} NodeStack;

void traverse_tree_iterative(TreeNode *root) {
    NodeStack stack = {.top = -1};
    
    // Push root
    stack.nodes[++stack.top] = root;
    
    while (stack.top >= 0) {
        TreeNode *node = stack.nodes[stack.top--];
        
        if (node == NULL) continue;
        
        process_node(node);
        
        // Push children (if space available)
        if (stack.top < MAX_STACK_SIZE - 1) {
            stack.nodes[++stack.top] = node->right;
        }
        if (stack.top < MAX_STACK_SIZE - 1) {
            stack.nodes[++stack.top] = node->left;
        }
    }
}
```

## 📊 Cas d'usage

### Parser de commandes

```c
typedef enum {
    CMD_START,
    CMD_STOP,
    CMD_PAUSE,
    CMD_UNKNOWN
} Command;

Command parse_command(const char *str) {
    if (strcmp(str, "START") == 0) return CMD_START;
    if (strcmp(str, "STOP") == 0) return CMD_STOP;
    if (strcmp(str, "PAUSE") == 0) return CMD_PAUSE;
    return CMD_UNKNOWN;
}

int execute_commands(char **commands, size_t count) {
    for (size_t i = 0; i < count; i++) {
        Command cmd = parse_command(commands[i]);
        
        switch (cmd) {
            case CMD_START:
                system_start();
                break;
            case CMD_STOP:
                system_stop();
                return 0;  // Early return
            case CMD_PAUSE:
                system_pause();
                break;
            case CMD_UNKNOWN:
                return -1;  // Error, early return
        }
    }
    return 0;
}
```

## 🔍 Détection

### Outils

```bash
# Rechercher goto
grep -n "goto" *.c

# Rechercher récursion
clang --analyze -Xanalyzer -analyzer-checker=core *.c

# Cppcheck
cppcheck --enable=all --force *.c
```

### Revue de code

- ✅ Flux de contrôle linéaire
- ✅ Maximum 3 niveaux de nesting
- ✅ Boucles simples (for/while)
- ✅ Pas d'appels récursifs

## 🎓 Exercices

Voir `exercises/ex01_control_flow.c`

## 📚 Références

- **JPL Coding Standard**: Rule 1
- **MISRA C**: Directive 4.6 (no recursion)
- **Linux Kernel**: goto for error handling only
- **Safety-critical standards**: IEC 61508, DO-178C

## 💡 Points clés

1. **Lisibilité**: Le code doit se lire de haut en bas
2. **Prédictibilité**: Chemin d'exécution clair
3. **Vérifiabilité**: Analyse statique possible
4. **Maintenabilité**: Modifications sans effet de bord

## ⚠️ Exceptions

- `goto cleanup`: Pattern accepté pour centraliser le nettoyage
- Tail recursion optimisée par compilateur (si garantie)
- Code généré automatiquement (parsers)

---

**Résumé**: Pas de goto, pas de récursion, flux linéaire et prédictible.
