# Règle 10 : Analyse Statique

## 📋 Énoncé

**Utiliser des outils d'analyse statique et corriger tous les problèmes détectés**
- Analyse statique obligatoire
- Tous les bugs détectés doivent être corrigés
- Vérification automatisée dans le build
- Pas de faux positifs tolérés sans justification

## 🎯 Objectif

- **Détection**: Trouver bugs avant exécution
- **Prévention**: Code vérifié formellement
- **Qualité**: Zéro défaut détectable
- **Automatisation**: Vérification continue

## 🔧 Outils Principaux

### 1. Clang Static Analyzer

```bash
# Analyse complète
clang --analyze \
      -Xanalyzer -analyzer-output=text \
      -Wall -Wextra \
      *.c

# Checkers spécifiques
clang --analyze \
      -Xanalyzer -analyzer-checker=core \
      -Xanalyzer -analyzer-checker=unix \
      -Xanalyzer -analyzer-checker=security \
      *.c
```

### 2. Cppcheck

```bash
# Analyse standard
cppcheck --enable=all \
         --suppress=missingIncludeSystem \
         --error-exitcode=1 \
         *.c

# Avec détails
cppcheck --enable=all \
         --verbose \
         --template='{file}:{line}: {severity}: {message}' \
         *.c
```

### 3. GCC avec sanitizers

```bash
# Undefined Behavior Sanitizer
gcc -Wall -Wextra -fsanitize=undefined -g program.c

# Address Sanitizer (memory errors)
gcc -Wall -Wextra -fsanitize=address -g program.c

# Leak Sanitizer
gcc -Wall -Wextra -fsanitize=leak -g program.c
```

## ❌ Problèmes Détectés

### 1. Null pointer dereference

```c
// ❌ MAUVAIS - Clang détecte
void bad_null_deref(Data *data) {
    if (data == NULL) {
        printf("Error\n");
        // ❌ Oubli de return!
    }
    
    data->value = 42;  // Warning: potential null dereference
}

// ✅ BON
void good_null_check(Data *data) {
    if (data == NULL) {
        printf("Error\n");
        return;  // ✅
    }
    
    data->value = 42;  // Safe
}
```

### 2. Memory leaks

```c
// ❌ MAUVAIS - Cppcheck/Clang détectent
void bad_memory_leak(void) {
    char *buffer = malloc(100);
    
    if (some_error()) {
        return;  // ❌ Leak!
    }
    
    free(buffer);
}

// ✅ BON - Pas de malloc (Rule 3)
#define BUFFER_SIZE 100

void good_static_buffer(void) {
    char buffer[BUFFER_SIZE];
    
    if (some_error()) {
        return;  // ✅ Pas de leak possible
    }
    
    // Use buffer...
}
```

### 3. Buffer overflow

```c
// ❌ MAUVAIS - Cppcheck détecte
void bad_buffer_overflow(void) {
    char buffer[10];
    
    strcpy(buffer, "This is a very long string");  // ❌ Overflow!
}

// ✅ BON
void good_buffer_safe(void) {
    char buffer[50];
    const char *input = "This is a very long string";
    
    assert(strlen(input) < sizeof(buffer));
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // Null-terminate
}
```

### 4. Use after free

```c
// ❌ MAUVAIS - ASan détecte
void bad_use_after_free(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    
    printf("%d\n", *ptr);  // ❌ Use after free!
}

// ✅ BON - Pas de malloc (Rule 3)
void good_static_allocation(void) {
    int value = 42;
    printf("%d\n", value);  // ✅ Safe
}
```

### 5. Uninitialized values

```c
// ❌ MAUVAIS - Valgrind/MSan détecte
void bad_uninitialized(void) {
    int array[10];
    
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);  // ❌ Valeurs non initialisées!
    }
}

// ✅ BON
void good_initialized(void) {
    int array[10] = {0};  // ✅ Initialisé
    
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
}
```

## 🎯 Problèmes Courants

### 1. Division par zéro

```c
// ✅ BON - Détecté par analyse statique
int safe_divide(int a, int b) {
    assert(b != 0);  // Clang static analyzer vérifie
    
    if (b == 0) {
        return 0;  // Error handling
    }
    
    return a / b;
}
```

### 2. Array out of bounds

```c
// ❌ MAUVAIS
void bad_bounds(void) {
    int array[10];
    array[10] = 42;  // ❌ Out of bounds!
}

// ✅ BON
void good_bounds(void) {
    int array[10];
    for (size_t i = 0; i < 10; i++) {  // ✅ Bounds checked
        array[i] = (int)i;
    }
}
```

### 3. Double free

```c
// ❌ MAUVAIS
void bad_double_free(void) {
    int *ptr = malloc(sizeof(int));
    free(ptr);
    free(ptr);  // ❌ Double free!
}

// ✅ BON - Pas de malloc (Rule 3)
// OU si vraiment nécessaire:
void good_free_pattern(void) {
    int *ptr = malloc(sizeof(int));
    if (ptr != NULL) {
        free(ptr);
        ptr = NULL;  // ✅ Évite double free
    }
}
```

### 4. Resource leaks

```c
// ❌ MAUVAIS
void bad_file_leak(void) {
    FILE *f = fopen("data.txt", "r");
    
    if (some_error()) {
        return;  // ❌ File leak!
    }
    
    fclose(f);
}

// ✅ BON
void good_file_handling(void) {
    FILE *f = fopen("data.txt", "r");
    if (f == NULL) {
        return;
    }
    
    bool error = false;
    
    if (some_error()) {
        error = true;
    } else {
        // Process file
    }
    
    fclose(f);  // ✅ Toujours fermé
    
    if (error) {
        // Handle error
    }
}
```

## 📊 Integration CI/CD

### Makefile avec analyse

```makefile
# Compilation normale
all: program

program: main.c utils.c
	$(CC) $(CFLAGS) -o $@ $^

# Analyse statique
analyze: static-clang static-cppcheck

static-clang:
	@echo "Running Clang Static Analyzer..."
	clang --analyze -Xanalyzer -analyzer-output=text $(CFLAGS) *.c

static-cppcheck:
	@echo "Running Cppcheck..."
	cppcheck --enable=all --error-exitcode=1 \
	         --suppress=missingIncludeSystem *.c

# Tests avec sanitizers
test-asan:
	$(CC) $(CFLAGS) -fsanitize=address -g -o test_asan *.c
	./test_asan

test-ubsan:
	$(CC) $(CFLAGS) -fsanitize=undefined -g -o test_ubsan *.c
	./test_ubsan

# Validation complète
validate: analyze test-asan test-ubsan
	@echo "✅ All static analysis checks passed!"

.PHONY: all analyze static-clang static-cppcheck test-asan test-ubsan validate
```

## 🎨 Patterns

### 1. Null check pattern

```c
// Pattern vérifié par analyseurs
bool process_data(const Data *data) {
    if (data == NULL) {
        return false;
    }
    
    // Analyseur sait que data != NULL ici
    use_data(data);  // ✅ Safe
    return true;
}
```

### 2. Error propagation

```c
// ✅ BON - Analyseur suit le flow
int open_and_process(const char *filename) {
    assert(filename != NULL);
    
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        return -1;  // Analyseur sait: f pas utilisé
    }
    
    int result = process_file(f);
    
    fclose(f);  // Analyseur vérifie: toujours fermé
    return result;
}
```

### 3. Bounded operations

```c
// ✅ BON - Bounds vérifiables statiquement
void safe_copy(char dest[static 64], const char *src) {
    assert(dest != NULL);
    assert(src != NULL);
    
    // Analyseur peut vérifier que dest a au moins 64 bytes
    for (size_t i = 0; i < 63; i++) {
        dest[i] = src[i];
        if (src[i] == '\0') {
            return;
        }
    }
    dest[63] = '\0';  // Force termination
}
```

## 🔬 Analyseur Checklist

### Core checkers

- [x] `core.DivideZero` - Division par zéro
- [x] `core.NullDereference` - Déréférence null
- [x] `core.StackAddressEscape` - Adresse stack qui s'échappe
- [x] `core.UndefinedBinaryOperatorResult` - Opération sur uninitialized
- [x] `core.uninitialized.Branch` - Branch sur condition non initialisée

### Unix checkers

- [x] `unix.Malloc` - Memory leaks, double free
- [x] `unix.API` - Utilisation incorrecte d'APIs

### Security checkers

- [x] `security.insecureAPI.strcpy` - strcpy dangereux
- [x] `security.FloatLoopCounter` - Float comme compteur

## 💡 Points clés

1. **Automatiser**: Intégrer dans Makefile/CI
2. **Corriger tout**: Zéro défaut toléré
3. **Multiples outils**: Clang + Cppcheck + Sanitizers
4. **Documentation**: Justifier suppressions rares
5. **Continuous**: Analyser à chaque commit

## ⚠️ Pièges à éviter

```c
// ❌ PIÈGE 1 - Supprimer warning sans corriger
#pragma GCC diagnostic ignored "-Wunused-variable"
int bad_suppress(void) {
    int unused = 5;  // Toujours mauvais!
    return 0;
}

// ✅ SOLUTION - Corriger le problème
int good_fix(void) {
    return 0;  // Variable supprimée
}

// ❌ PIÈGE 2 - Cast pour masquer
void bad_cast_hide(void) {
    void *ptr = NULL;
    int *iptr = (int *)ptr;  // Masque warning mais toujours NULL!
    *iptr = 42;  // Crash!
}

// ✅ SOLUTION - Vérifier
void good_check(void) {
    void *ptr = get_pointer();
    if (ptr == NULL) {
        return;
    }
    int *iptr = (int *)ptr;
    *iptr = 42;  // Safe
}
```

## 📋 Workflow Complet

```bash
# 1. Compilation stricte
make CFLAGS="-Wall -Wextra -Werror -pedantic -std=c11"

# 2. Analyse statique Clang
make static-clang

# 3. Analyse Cppcheck
make static-cppcheck

# 4. Tests avec AddressSanitizer
make test-asan

# 5. Tests avec UndefinedBehaviorSanitizer
make test-ubsan

# 6. Valgrind (si malloc utilisé)
valgrind --leak-check=full --error-exitcode=1 ./program

# 7. Validation complète
make validate
```

## 🎓 Exemples de Bugs Détectés

### 1. Logic errors

```c
// Clang détecte:
void bad_logic(int *ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
    
    *ptr = 42;  // ❌ Warning: use after potential NULL
}
```

### 2. Dead code

```c
// Cppcheck détecte:
int bad_dead_code(int x) {
    if (x < 0) {
        return -1;
    } else {
        return 1;
    }
    
    return 0;  // ❌ Warning: unreachable code
}
```

### 3. Redundant conditions

```c
// Analyseur détecte:
void bad_redundant(unsigned int x) {
    if (x >= 0) {  // ❌ Warning: always true (unsigned!)
        process(x);
    }
}
```

## 🏗️ Configuration Project

### .clang-tidy

```yaml
---
Checks: 'clang-analyzer-*,
         cppcoreguidelines-*,
         readability-*,
         performance-*,
         portability-*,
         bugprone-*'
         
WarningsAsErrors: '*'

CheckOptions:
  - key: readability-function-size.LineThreshold
    value: 60
```

### Makefile complet

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -g

# Tous les fichiers
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

# Programme principal
program: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Analyse statique complète
analyze: clean
	@echo "=== Clang Static Analyzer ==="
	@clang --analyze -Xanalyzer -analyzer-output=text $(CFLAGS) $(SOURCES) || \
	    (echo "❌ Clang analysis failed" && exit 1)
	@echo "✅ Clang analysis passed\n"
	
	@echo "=== Cppcheck ==="
	@cppcheck --enable=all --error-exitcode=1 \
	          --suppress=missingIncludeSystem $(SOURCES) || \
	    (echo "❌ Cppcheck failed" && exit 1)
	@echo "✅ Cppcheck passed\n"

# Tests avec sanitizers
test-sanitizers: clean
	@echo "=== Address Sanitizer ==="
	@$(CC) $(CFLAGS) -fsanitize=address -o test_asan $(SOURCES)
	@./test_asan || (echo "❌ ASan failed" && exit 1)
	@echo "✅ ASan passed\n"
	
	@echo "=== Undefined Behavior Sanitizer ==="
	@$(CC) $(CFLAGS) -fsanitize=undefined -o test_ubsan $(SOURCES)
	@./test_ubsan || (echo "❌ UBSan failed" && exit 1)
	@echo "✅ UBSan passed\n"

# Validation complète
validate: analyze test-sanitizers
	@echo "🎉 All validation checks passed!"

clean:
	rm -f *.o *.plist program test_asan test_ubsan

.PHONY: all analyze test-sanitizers validate clean
```

## 📝 Types de Défauts

| Catégorie | Exemples | Outil |
|-----------|----------|-------|
| **Memory** | Null deref, use-after-free, leaks | Clang, ASan |
| **Logic** | Dead code, redundant checks | Clang, Cppcheck |
| **Bounds** | Buffer overflow, array access | Cppcheck, ASan |
| **Undefined** | Signed overflow, shift | UBSan, Clang |
| **Resources** | File/socket leaks | Clang, Cppcheck |
| **Concurrency** | Race conditions | ThreadSanitizer |

## 🧪 Exemple Complet

```c
// Code analysé statiquement
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define MAX_SIZE 100

typedef struct {
    int values[MAX_SIZE];
    size_t count;
} SafeArray;

bool array_add(SafeArray *array, int value) {
    // Préconditions vérifiées statiquement
    assert(array != NULL);
    assert(array->count <= MAX_SIZE);
    
    if (array->count >= MAX_SIZE) {
        return false;  // Clang vérifie: pas de buffer overflow
    }
    
    array->values[array->count] = value;  // Safe
    array->count++;
    
    // Postcondition vérifiable
    assert(array->count <= MAX_SIZE);
    return true;
}

int array_get(const SafeArray *array, size_t index) {
    assert(array != NULL);
    assert(index < array->count);  // Clang vérifie bounds
    
    return array->values[index];  // Safe
}

int main(void) {
    SafeArray arr = {.values = {0}, .count = 0};
    
    // Usage vérifié
    bool ok = array_add(&arr, 42);
    assert(ok);
    
    int value = array_get(&arr, 0);
    printf("Value: %d\n", value);
    
    return 0;
}
```

### Analyse complète

```bash
# 1. Compilation stricte
$ gcc -Wall -Wextra -Werror -pedantic -std=c11 -c safe_array.c
# ✅ 0 warnings

# 2. Clang analyzer
$ clang --analyze safe_array.c
# ✅ 0 bugs found

# 3. Cppcheck
$ cppcheck --enable=all safe_array.c
# ✅ 0 errors

# 4. Runtime avec ASan
$ gcc -fsanitize=address -g safe_array.c && ./a.out
# ✅ No memory errors

# 5. Runtime avec UBSan
$ gcc -fsanitize=undefined -g safe_array.c && ./a.out
# ✅ No undefined behavior
```

## 🚀 Automatisation

### Script CI

```bash
#!/bin/bash
# ci_static_analysis.sh

set -e  # Exit on error

echo "🔍 Running static analysis..."

# Compile with strict flags
gcc -Wall -Wextra -Werror -pedantic -std=c11 -c *.c

# Clang static analyzer
clang --analyze -Xanalyzer -analyzer-output=text *.c
if [ $? -ne 0 ]; then
    echo "❌ Static analysis failed"
    exit 1
fi

# Cppcheck
cppcheck --enable=all --error-exitcode=1 \
         --suppress=missingIncludeSystem *.c
if [ $? -ne 0 ]; then
    echo "❌ Cppcheck failed"
    exit 1
fi

echo "✅ Static analysis passed!"
```

## 💡 Points clés

1. **Multiples outils**: Clang + Cppcheck + Sanitizers
2. **Automatisation**: Dans Makefile et CI
3. **Zéro défaut**: Tous les bugs corrigés
4. **Assertions**: Aident l'analyse statique
5. **Documentation**: Justifier les suppressions

## ⚠️ Limitations

- Faux positifs possibles (rares)
- Nécessite code bien structuré
- Peut ralentir build (acceptable)
- Ne remplace pas les tests

## 🔗 Resources

- Clang Static Analyzer: https://clang-analyzer.llvm.org
- Cppcheck: http://cppcheck.sourceforge.net
- Sanitizers: https://github.com/google/sanitizers

## 🎓 Exercices

Voir `exercises/ex10_static_analysis.c` - Corriger problèmes détectés par analyse statique

---

**Résumé**: Utiliser Clang analyzer, Cppcheck, et sanitizers. Corriger tous les défauts détectés. Automatiser dans CI/CD.
