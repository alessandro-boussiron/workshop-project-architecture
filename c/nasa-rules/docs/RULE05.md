# Règle 5 : Vérifier les Valeurs de Retour

## 📋 Énoncé

**Toutes les valeurs de retour des fonctions non-void doivent être vérifiées**
- Vérifier TOUS les retours de fonctions
- Gérer TOUS les cas d'erreur
- Pas d'ignorance silencieuse des erreurs

## 🎯 Objectif

- **Détection précoce**: Attraper les erreurs immédiatement
- **Robustesse**: Gérer tous les cas d'échec
- **Traçabilité**: Savoir quand/où une erreur survient
- **Récupération**: Possibilité de récupérer gracieusement

## ❌ Problèmes

### 1. Ignorer les retours de fonctions système

```c
// ❌ MAUVAIS
void bad_file_operations(const char *filename) {
    FILE *file = fopen(filename, "r");  // Pas de vérification!
    
    char buffer[100];
    fgets(buffer, sizeof(buffer), file);  // file peut être NULL!
    
    printf("%s\n", buffer);
    
    fclose(file);  // Résultat ignoré
}
```

### 2. Ignorer malloc

```c
// ❌ MAUVAIS
void bad_allocation(size_t size) {
    int *array = malloc(size * sizeof(int));  // Pas de vérification!
    
    for (size_t i = 0; i < size; i++) {
        array[i] = i;  // Crash si malloc a échoué!
    }
    
    free(array);
}
```

### 3. Utiliser atoi sans vérification

```c
// ❌ MAUVAIS
void bad_string_conversion(const char *str) {
    int value = atoi(str);  // Pas de vérification d'erreur possible!
    
    // Si str = "abc", value = 0 (indistinguable d'un vrai 0)
    process_value(value);
}
```

### 4. Chaîner sans vérifier

```c
// ❌ MAUVAIS
void bad_chained_operations(void) {
    FILE *file = fopen("data.txt", "r");
    char line[100];
    fgets(line, sizeof(line), file);  // file peut être NULL!
    
    int value = atoi(line);  // line peut être garbage
    printf("Value: %d\n", value);
    
    fclose(file);
}
```

## ✅ Solutions

### 1. Vérification systématique des fichiers

```c
// ✅ BON
typedef enum {
    FILE_OK = 0,
    FILE_ERROR_OPEN,
    FILE_ERROR_READ,
    FILE_ERROR_CLOSE
} FileError;

FileError safe_file_operations(const char *filename) {
    if (filename == NULL) {
        return FILE_ERROR_OPEN;
    }
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return FILE_ERROR_OPEN;
    }
    
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fprintf(stderr, "Error: Cannot read from %s\n", filename);
        fclose(file);
        return FILE_ERROR_READ;
    }
    
    printf("%s\n", buffer);
    
    if (fclose(file) != 0) {
        fprintf(stderr, "Warning: Error closing %s\n", filename);
        return FILE_ERROR_CLOSE;
    }
    
    return FILE_OK;
}
```

### 2. Vérification malloc/calloc

```c
// ✅ BON
int* safe_allocation(size_t size) {
    if (size == 0 || size > MAX_ALLOCATION_SIZE) {
        return NULL;
    }
    
    int *array = malloc(size * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    // Initialiser
    for (size_t i = 0; i < size; i++) {
        array[i] = 0;
    }
    
    return array;
}

bool process_with_allocation(size_t size) {
    int *array = safe_allocation(size);
    if (array == NULL) {
        return false;  // Échec d'allocation géré
    }
    
    // Traitement...
    
    free(array);
    return true;
}
```

### 3. Conversion string → int sécurisée

```c
// ✅ BON - Utiliser strtol
#include <errno.h>
#include <limits.h>

bool safe_string_to_int(const char *str, int *result) {
    if (str == NULL || result == NULL) {
        return false;
    }
    
    char *endptr;
    errno = 0;
    
    long value = strtol(str, &endptr, 10);
    
    // Vérifications multiples
    if (errno == ERANGE) {
        return false;  // Overflow/underflow
    }
    
    if (endptr == str) {
        return false;  // Pas de conversion
    }
    
    if (*endptr != '\0') {
        return false;  // Caractères restants
    }
    
    if (value < INT_MIN || value > INT_MAX) {
        return false;  // Hors limites int
    }
    
    *result = (int)value;
    return true;
}
```

### 4. Opérations chaînées sécurisées

```c
// ✅ BON
typedef enum {
    OP_OK = 0,
    OP_FILE_ERROR,
    OP_READ_ERROR,
    OP_PARSE_ERROR
} OperationStatus;

OperationStatus safe_chained_operations(const char *filename, int *result) {
    if (filename == NULL || result == NULL) {
        return OP_FILE_ERROR;
    }
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return OP_FILE_ERROR;
    }
    
    char line[100];
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return OP_READ_ERROR;
    }
    
    int value;
    if (!safe_string_to_int(line, &value)) {
        fclose(file);
        return OP_PARSE_ERROR;
    }
    
    *result = value;
    
    fclose(file);
    return OP_OK;
}
```

## 📊 Patterns Avancés

### 1. Error Cleanup Pattern

```c
// ✅ BON - Cleanup centralisé
typedef enum {
    SUCCESS = 0,
    ERROR_FILE_A,
    ERROR_FILE_B,
    ERROR_MEMORY,
    ERROR_PROCESSING
} Status;

Status complex_operation(const char *file_a, const char *file_b) {
    FILE *fa = NULL;
    FILE *fb = NULL;
    char *buffer = NULL;
    Status status = SUCCESS;
    
    // Ouvrir fichier A
    fa = fopen(file_a, "r");
    if (fa == NULL) {
        status = ERROR_FILE_A;
        goto cleanup;
    }
    
    // Ouvrir fichier B
    fb = fopen(file_b, "w");
    if (fb == NULL) {
        status = ERROR_FILE_B;
        goto cleanup;
    }
    
    // Allouer buffer
    buffer = malloc(1024);
    if (buffer == NULL) {
        status = ERROR_MEMORY;
        goto cleanup;
    }
    
    // Traitement
    if (!process_files(fa, fb, buffer)) {
        status = ERROR_PROCESSING;
        goto cleanup;
    }
    
cleanup:
    free(buffer);
    if (fb != NULL) fclose(fb);
    if (fa != NULL) fclose(fa);
    
    return status;
}
```

### 2. Result Type Pattern

```c
// ✅ BON - Type résultat avec erreur
typedef struct {
    int value;
    bool valid;
    const char *error_message;
} Result;

Result safe_divide(int a, int b) {
    Result result = {0};
    
    if (b == 0) {
        result.valid = false;
        result.error_message = "Division by zero";
        return result;
    }
    
    result.value = a / b;
    result.valid = true;
    result.error_message = NULL;
    
    return result;
}

void use_result(void) {
    Result res = safe_divide(10, 0);
    
    if (!res.valid) {
        fprintf(stderr, "Error: %s\n", res.error_message);
        return;
    }
    
    printf("Result: %d\n", res.value);
}
```

### 3. Multiple Error Codes

```c
// ✅ BON - Codes d'erreur détaillés
typedef enum {
    SENSOR_OK = 0,
    SENSOR_NOT_FOUND = -1,
    SENSOR_TIMEOUT = -2,
    SENSOR_INVALID_DATA = -3,
    SENSOR_CALIBRATION_ERROR = -4
} SensorStatus;

const char* sensor_status_string(SensorStatus status) {
    switch (status) {
        case SENSOR_OK: return "OK";
        case SENSOR_NOT_FOUND: return "Sensor not found";
        case SENSOR_TIMEOUT: return "Timeout";
        case SENSOR_INVALID_DATA: return "Invalid data";
        case SENSOR_CALIBRATION_ERROR: return "Calibration error";
        default: return "Unknown error";
    }
}

SensorStatus read_sensor(int sensor_id, float *value) {
    if (value == NULL) {
        return SENSOR_INVALID_DATA;
    }
    
    if (sensor_id < 0 || sensor_id >= MAX_SENSORS) {
        return SENSOR_NOT_FOUND;
    }
    
    if (!sensor_wait_ready(sensor_id)) {
        return SENSOR_TIMEOUT;
    }
    
    int raw = sensor_read_raw(sensor_id);
    if (raw < 0) {
        return SENSOR_INVALID_DATA;
    }
    
    *value = sensor_convert(raw);
    return SENSOR_OK;
}

void process_sensor(int sensor_id) {
    float value;
    SensorStatus status = read_sensor(sensor_id, &value);
    
    if (status != SENSOR_OK) {
        fprintf(stderr, "Sensor %d error: %s\n", 
                sensor_id, sensor_status_string(status));
        return;
    }
    
    printf("Sensor %d: %.2f\n", sensor_id, value);
}
```

### 4. Assertions + Return Checks

```c
// ✅ BON - Double vérification
bool critical_operation(const Data *data, Result *result) {
    // Assertions pour bugs de programmation
    assert(data != NULL);
    assert(result != NULL);
    
    // Vérification runtime pour erreurs externes
    if (!data->valid) {
        return false;
    }
    
    int status = external_function(data->value);
    if (status != 0) {
        log_error("External function failed: %d", status);
        return false;
    }
    
    result->success = true;
    return true;
}
```

## 🔍 Détection

### Static Analysis

```bash
# Clang-tidy
clang-tidy -checks='bugprone-unused-return-value' *.c

# Cppcheck
cppcheck --enable=warning --inconclusive *.c

# GCC warnings
gcc -Wall -Wextra -Wunused-result *.c
```

### Code Review Checklist

```c
// Pour chaque appel de fonction:
// [ ] Valeur de retour vérifiée
// [ ] Cas d'erreur géré
// [ ] Ressources nettoyées en cas d'erreur
// [ ] Erreur loggée ou rapportée
```

## 📚 Fonctions Système Critiques

### À TOUJOURS vérifier

```c
// Fichiers
FILE *f = fopen(...);     // Vérifier != NULL
fread(...);               // Vérifier bytes read
fwrite(...);              // Vérifier bytes written
fclose(...);              // Vérifier == 0

// Mémoire
void *p = malloc(...);    // Vérifier != NULL
void *p = calloc(...);    // Vérifier != NULL
void *p = realloc(...);   // Vérifier != NULL

// Strings
char *s = strdup(...);    // Vérifier != NULL
char *t = strtok(...);    // Vérifier != NULL
long l = strtol(...);     // Vérifier errno

// Système
int fd = open(...);       // Vérifier != -1
ssize_t n = read(...);    // Vérifier > 0
int r = close(...);       // Vérifier == 0
```

## 💡 Points clés

1. **Vérifier TOUJOURS**: Aucune exception
2. **Early return**: Sortir tôt en cas d'erreur
3. **Cleanup**: Libérer ressources sur tous les chemins
4. **Log**: Enregistrer les erreurs pour debug
5. **errno**: Vérifier errno pour fonctions système

## 🎓 Exercices

Voir `exercises/ex05_check_returns.c`

---

**Résumé**: Vérifier toutes les valeurs de retour, gérer tous les cas d'erreur, nettoyer les ressources.
