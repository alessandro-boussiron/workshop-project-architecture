# Règle 6 : Limiter la Portée des Variables

## 📋 Énoncé

**Déclarer les données dans la plus petite portée possible**
- Variables déclarées au plus proche de leur utilisation
- Éviter les variables globales
- Limiter la durée de vie des variables
- Minimiser les dépendances

## 🎯 Objectif

- **Encapsulation**: Variables accessibles uniquement où nécessaire
- **Lisibilité**: Comprendre l'usage d'une variable immédiatement
- **Sécurité**: Éviter les modifications accidentelles
- **Maintenabilité**: Changements localisés

## ❌ Problèmes

### 1. Variables globales partout

```c
// ❌ MAUVAIS
int g_sensor_id;          // Global
int g_last_reading;       // Global
bool g_is_calibrated;     // Global
float g_calibration_offset; // Global

void init_sensor(void) {
    g_sensor_id = 1;
    g_is_calibrated = false;
}

void read_sensor(void) {
    g_last_reading = read_adc(g_sensor_id);
}

void calibrate(void) {
    g_calibration_offset = calculate_offset(g_last_reading);
    g_is_calibrated = true;
}
```

**Problèmes:**
- État global = bugs difficiles à tracer
- Impossible à tester indépendamment
- Pas de réentrance
- Conflits de nommage

### 2. Variables déclarées trop tôt

```c
// ❌ MAUVAIS
void bad_function(int *data, size_t size) {
    int i, j, k;        // Déclarées au début
    int temp;
    int sum;
    double average;
    int max, min;
    
    // 20 lignes de code...
    
    // i utilisé ici pour la première fois
    for (i = 0; i < size; i++) {
        sum += data[i];
    }
    
    // 10 lignes...
    
    // j utilisé ici
    for (j = 0; j < size; j++) {
        temp = data[j];
        // ...
    }
    
    // k jamais utilisé!
}
```

### 3. Réutilisation de variables

```c
// ❌ MAUVAIS
void bad_reuse(int *array, size_t size) {
    int temp;  // Variable passe-partout
    
    // Usage 1: pour swap
    temp = array[0];
    array[0] = array[1];
    array[1] = temp;
    
    // Usage 2: pour somme
    temp = 0;
    for (size_t i = 0; i < size; i++) {
        temp += array[i];
    }
    
    // Usage 3: pour minimum
    temp = array[0];
    for (size_t i = 1; i < size; i++) {
        if (array[i] < temp) {
            temp = array[i];
        }
    }
}
```

## ✅ Solutions

### 1. État dans une structure

```c
// ✅ BON - État encapsulé
typedef struct {
    int sensor_id;
    int last_reading;
    bool is_calibrated;
    float calibration_offset;
} SensorState;

void init_sensor(SensorState *state, int sensor_id) {
    assert(state != NULL);
    
    state->sensor_id = sensor_id;
    state->last_reading = 0;
    state->is_calibrated = false;
    state->calibration_offset = 0.0f;
}

void read_sensor(SensorState *state) {
    assert(state != NULL);
    
    state->last_reading = read_adc(state->sensor_id);
}

void calibrate(SensorState *state) {
    assert(state != NULL);
    
    state->calibration_offset = 
        calculate_offset(state->last_reading);
    state->is_calibrated = true;
}
```

### 2. Variables au plus proche de l'utilisation

```c
// ✅ BON - Portée minimale
void good_function(int *data, size_t size) {
    // Calcul de la somme
    {
        int sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += data[i];
        }
        double average = (double)sum / size;
        printf("Average: %.2f\n", average);
    }
    
    // Recherche du maximum
    {
        int max = data[0];
        for (size_t i = 1; i < size; i++) {
            if (data[i] > max) {
                max = data[i];
            }
        }
        printf("Max: %d\n", max);
    }
    
    // Tri (variables dans la boucle)
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                int temp = data[j];  // Portée limitée
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}
```

### 3. Variables spécifiques

```c
// ✅ BON - Noms significatifs, portées distinctes
void good_array_operations(int *array, size_t size) {
    // Swap - variables dédiées
    {
        int swap_temp = array[0];
        array[0] = array[1];
        array[1] = swap_temp;
    }
    
    // Sum - variable dédiée
    {
        int sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += array[i];
        }
        printf("Sum: %d\n", sum);
    }
    
    // Min - variable dédiée
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

## 📊 Patterns

### 1. Variables dans les boucles (C99+)

```c
// ✅ BON - Variable de boucle dans for()
void process_data(const int *data, size_t size) {
    // 'i' existe uniquement dans cette boucle
    for (size_t i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    
    // 'i' peut être réutilisé, portée différente
    for (size_t i = 0; i < size; i++) {
        if (data[i] < 0) {
            break;
        }
    }
}
```

### 2. Block Scope

```c
// ✅ BON - Utiliser { } pour limiter
void calculate_statistics(const int *data, size_t size) {
    // Mean in its own scope
    {
        long sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += data[i];
        }
        double mean = (double)sum / size;
        printf("Mean: %.2f\n", mean);
    }
    // sum et mean n'existent plus ici
    
    // Variance in its own scope
    {
        double mean = calculate_mean(data, size);
        double sum_squared_diff = 0.0;
        
        for (size_t i = 0; i < size; i++) {
            double diff = data[i] - mean;
            sum_squared_diff += diff * diff;
        }
        
        double variance = sum_squared_diff / size;
        printf("Variance: %.2f\n", variance);
    }
    // Variables variance scope n'existent plus
}
```

### 3. Static pour limiter portée fichier

```c
// ✅ BON - static = portée fichier uniquement

// Visible uniquement dans ce fichier
static SensorState g_primary_sensor = {0};

// Helper visible uniquement dans ce fichier
static float convert_to_celsius(int raw_value) {
    return (raw_value - 32) * 5.0f / 9.0f;
}

// API publique
void public_init_sensor(void) {
    init_sensor(&g_primary_sensor, 1);
}
```

### 4. Éviter variables de classe/module

```c
// ❌ MAUVAIS - Variables de module
static int module_counter = 0;
static bool module_initialized = false;

void module_function_a(void) {
    module_counter++;  // Couplage!
}

void module_function_b(void) {
    module_counter++;  // Couplage!
}

// ✅ BON - Passer contexte explicitement
typedef struct {
    int counter;
    bool initialized;
} ModuleContext;

void module_init(ModuleContext *ctx) {
    ctx->counter = 0;
    ctx->initialized = true;
}

void module_function_a(ModuleContext *ctx) {
    ctx->counter++;
}

void module_function_b(ModuleContext *ctx) {
    ctx->counter++;
}
```

## 🔍 Analyse

### Durée de vie des variables

```c
void demonstrate_lifetime(void) {
    // 'a' vit pendant toute la fonction
    int a = 10;
    
    {
        // 'b' vit uniquement dans ce bloc
        int b = 20;
        printf("a=%d, b=%d\n", a, b);
    }
    // 'b' n'existe plus ici
    
    for (int i = 0; i < 5; i++) {
        // 'i' vit uniquement dans la boucle
        printf("%d\n", i);
    }
    // 'i' n'existe plus ici
}
```

### Distance entre déclaration et usage

```c
// ❌ MAUVAIS - Grande distance
void bad_distance(void) {
    int result;  // Déclaré ici
    
    // 50 lignes de code...
    
    result = calculate();  // Utilisé ici (loin!)
}

// ✅ BON - Petite distance
void good_distance(void) {
    // 50 lignes de code...
    
    int result = calculate();  // Déclaré et initialisé ensemble
    use_result(result);
}
```

## 🛠️ Outils

### Static Analysis

```bash
# Cppcheck - variables inutilisées
cppcheck --enable=style *.c

# Clang-tidy - scope issues
clang-tidy -checks='readability-isolate-declaration' *.c

# GCC warnings
gcc -Wall -Wextra -Wshadow *.c
```

### Code Metrics

```bash
# Compter variables globales
grep -n "^[a-z_][a-z0-9_]* [a-z]" *.c

# Compter static globals
grep -n "^static.*;" *.c | grep -v "static.*("
```

## 💡 Points clés

1. **Déclarer au plus proche**: Juste avant utilisation
2. **Initialiser à la déclaration**: `int x = 0;` pas `int x; x = 0;`
3. **Utiliser block scope**: `{ }` pour limiter
4. **Passer par paramètres**: Pas de globals
5. **static pour limiter**: Visibilité fichier uniquement
6. **const pour protéger**: Variables non modifiables

## ⚠️ Pièges

### Shadow Variables

```c
// ⚠️ ATTENTION - Variable shadowing
int x = 10;  // Variable externe

void function(void) {
    printf("%d\n", x);  // Affiche 10
    
    {
        int x = 20;  // Shadow la variable externe!
        printf("%d\n", x);  // Affiche 20
    }
    
    printf("%d\n", x);  // Affiche 10 à nouveau
}

// ✅ MEILLEUR - Noms distincts
int outer_value = 10;

void function(void) {
    printf("%d\n", outer_value);  // Clair
    
    {
        int inner_value = 20;  // Pas de confusion
        printf("%d\n", inner_value);
    }
}
```

## 🎓 Exercices

Voir `exercises/ex06_limit_scope.c`

---

**Résumé**: Déclarer au plus proche, éviter globals, utiliser block scope, passer par paramètres.
