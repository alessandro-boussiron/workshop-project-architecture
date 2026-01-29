# Règle 2 : Bornes Fixes pour les Boucles

## 📋 Énoncé

**Toutes les boucles doivent avoir une borne supérieure fixe**
- Chaque boucle doit avoir un maximum d'itérations déterminable statiquement
- Pas de boucles infinies
- Pas de boucles basées uniquement sur des conditions dynamiques

## 🎯 Objectif

Garantir que **toutes les boucles terminent** et permettre le calcul du **temps d'exécution maximum**.

## ❌ Problèmes

### 1. Boucle sans borne

```c
// ❌ MAUVAIS
void wait_for_ready(volatile bool *ready) {
    while (!*ready) {
        // Attente infinie si ready jamais mis à true!
    }
}
```

### 2. Boucle sur condition externe

```c
// ❌ MAUVAIS
void read_until_sentinel(uint8_t *stream) {
    int i = 0;
    while (stream[i] != 0xFF) {  // Et si pas de sentinel?
        process_byte(stream[i]);
        i++;
    }
}
```

### 3. Boucle sur validation de données

```c
// ❌ MAUVAIS
int find_value(int *array, int value) {
    int i = 0;
    while (array[i] != value) {  // Et si value pas dans array?
        i++;
    }
    return i;
}
```

### 4. Boucle infinie intentionnelle

```c
// ❌ MAUVAIS (même si intentionnel)
void main_loop(void) {
    while (1) {  // Pas de borne!
        process_events();
        update_state();
        render();
    }
}
```

## ✅ Solutions

### 1. Timeout avec compteur

```c
// ✅ BON
#define MAX_WAIT_CYCLES 1000

bool wait_for_ready_bounded(volatile bool *ready) {
    for (int i = 0; i < MAX_WAIT_CYCLES; i++) {
        if (*ready) {
            return true;  // Success
        }
        // Small delay
    }
    return false;  // Timeout
}
```

### 2. Taille maximum avec sentinel optionnel

```c
// ✅ BON
#define MAX_STREAM_SIZE 512

int read_stream_bounded(uint8_t *stream, size_t max_size) {
    const size_t safe_size = (max_size > MAX_STREAM_SIZE) ? 
                             MAX_STREAM_SIZE : max_size;
    
    int count = 0;
    for (size_t i = 0; i < safe_size; i++) {
        if (stream[i] == 0xFF) {
            break;  // Sentinel found
        }
        process_byte(stream[i]);
        count++;
    }
    return count;
}
```

### 3. Recherche bornée

```c
// ✅ BON
#define MAX_ARRAY_SIZE 100

int find_value_bounded(int *array, size_t size, int value) {
    const size_t safe_size = (size > MAX_ARRAY_SIZE) ? 
                             MAX_ARRAY_SIZE : size;
    
    for (size_t i = 0; i < safe_size; i++) {
        if (array[i] == value) {
            return (int)i;
        }
    }
    return -1;  // Not found
}
```

### 4. Main loop avec compteur d'itérations

```c
// ✅ BON
#define MAX_ITERATIONS 10000

void main_loop_bounded(void) {
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        process_events();
        update_state();
        render();
        
        if (should_exit()) {
            break;  // Early exit OK
        }
    }
}
```

## 📊 Patterns Courants

### Ring Buffer

```c
#define BUFFER_SIZE 16

typedef struct {
    int data[BUFFER_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} RingBuffer;

void ring_buffer_add(RingBuffer *buf, int value) {
    assert(buf->count < BUFFER_SIZE);  // Precondition
    
    buf->data[buf->tail] = value;
    buf->tail = (buf->tail + 1) % BUFFER_SIZE;
    buf->count++;
}

int ring_buffer_get(RingBuffer *buf) {
    assert(buf->count > 0);  // Precondition
    
    int value = buf->data[buf->head];
    buf->head = (buf->head + 1) % BUFFER_SIZE;
    buf->count--;
    
    return value;
}
```

### Moving Average Filter

```c
#define WINDOW_SIZE 8

typedef struct {
    int samples[WINDOW_SIZE];
    size_t index;
    size_t count;
    long sum;
} MovingAverage;

void moving_avg_add(MovingAverage *avg, int sample) {
    // Remove old value
    if (avg->count == WINDOW_SIZE) {
        avg->sum -= avg->samples[avg->index];
    }
    
    // Add new value
    avg->samples[avg->index] = sample;
    avg->sum += sample;
    
    // Update counters (bounded!)
    avg->index = (avg->index + 1) % WINDOW_SIZE;
    if (avg->count < WINDOW_SIZE) {
        avg->count++;
    }
}

int moving_avg_get(const MovingAverage *avg) {
    if (avg->count == 0) return 0;
    return (int)(avg->sum / avg->count);
}
```

### Bounded String Operations

```c
#define MAX_STRING_LENGTH 256

size_t safe_strlen(const char *str, size_t max_len) {
    size_t len = 0;
    
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    
    return len;
}

bool safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return false;
    }
    
    size_t i = 0;
    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    
    return (src[i] == '\0');  // True if complete copy
}
```

### Matrix Operations

```c
#define MAX_MATRIX_SIZE 10

typedef struct {
    int data[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    size_t rows;
    size_t cols;
} Matrix;

void matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result) {
    assert(a->cols == b->rows);
    assert(result->rows == a->rows);
    assert(result->cols == b->cols);
    
    // All loops bounded by constants
    for (size_t i = 0; i < result->rows && i < MAX_MATRIX_SIZE; i++) {
        for (size_t j = 0; j < result->cols && j < MAX_MATRIX_SIZE; j++) {
            int sum = 0;
            for (size_t k = 0; k < a->cols && k < MAX_MATRIX_SIZE; k++) {
                sum += a->data[i][k] * b->data[k][j];
            }
            result->data[i][j] = sum;
        }
    }
}
```

## 🔍 Analyse

### Calcul du temps maximum

```c
// Complexité temporelle calculable
void process_data(int *data, size_t size) {
    const size_t MAX_SIZE = 1000;
    const size_t safe_size = (size > MAX_SIZE) ? MAX_SIZE : size;
    
    // O(n) where n <= MAX_SIZE
    for (size_t i = 0; i < safe_size; i++) {
        data[i] *= 2;  // O(1)
    }
    
    // Temps max = MAX_SIZE * temps_operation
    // = 1000 * 5 cycles = 5000 cycles
}
```

### Boucles imbriquées

```c
#define MAX_OUTER 20
#define MAX_INNER 30

void nested_loops_bounded(void) {
    // Temps max = MAX_OUTER * MAX_INNER * temps_operation
    // = 20 * 30 * 10 = 6000 cycles
    
    for (int i = 0; i < MAX_OUTER; i++) {
        for (int j = 0; j < MAX_INNER; j++) {
            process(i, j);  // O(1)
        }
    }
}
```

## 🎓 Exercices

Voir `exercises/ex02_loop_bounds.c`

## 🛠️ Outils de vérification

### Static Analysis

```bash
# Cppcheck
cppcheck --enable=all --force *.c

# Clang analyzer
clang --analyze *.c

# CBMC (Bounded Model Checking)
cbmc --unwind 10 --unwinding-assertions file.c
```

### Code Review Checklist

```c
// ✅ Pour chaque boucle, vérifier:
// [ ] Borne supérieure définie par constante ou paramètre
// [ ] Pas de while(1) sans compteur
// [ ] Pas de condition uniquement externe
// [ ] Temps d'exécution maximum calculable
```

## 📚 Cas d'usage Real-World

### Télémétrie spatiale

```c
#define MAX_TELEMETRY_SAMPLES 100

typedef struct {
    double temperature[MAX_TELEMETRY_SAMPLES];
    uint32_t timestamps[MAX_TELEMETRY_SAMPLES];
    size_t count;
} TelemetryBuffer;

void process_telemetry(TelemetryBuffer *buffer) {
    // Borne fixe garantie
    for (size_t i = 0; i < buffer->count && i < MAX_TELEMETRY_SAMPLES; i++) {
        if (buffer->temperature[i] > 100.0) {
            trigger_alarm(buffer->timestamps[i]);
        }
    }
}
```

### Contrôle moteur

```c
#define MAX_CONTROL_CYCLES 50

void motor_control_loop(Motor *motor) {
    for (int cycle = 0; cycle < MAX_CONTROL_CYCLES; cycle++) {
        read_sensors(motor);
        calculate_pid(motor);
        update_pwm(motor);
        
        if (motor->target_reached) {
            break;  // Early exit OK
        }
        
        delay_microseconds(1000);
    }
}
```

## 💡 Points clés

1. **Toujours définir MAX_*** : Constantes pour bornes supérieures
2. **Clamp les entrées** : `size = min(size, MAX_SIZE)`
3. **Double condition** : `while (condition && count < MAX)`
4. **Timeout systématique** : Jamais d'attente infinie
5. **Calculer WCET** : Worst Case Execution Time

## ⚠️ Pièges à éviter

```c
// ❌ Borne dépendante de données
for (int i = 0; i < array[0]; i++) { }  // array[0] peut être n'importe quoi!

// ✅ Clamp la borne
int bound = (array[0] > MAX) ? MAX : array[0];
for (int i = 0; i < bound; i++) { }

// ❌ Boucle sur pointeur
while (*ptr++) { }  // Quand s'arrête-t-elle?

// ✅ Boucle bornée avec pointeur
for (int i = 0; i < MAX && ptr[i]; i++) { }
```

---

**Résumé**: Toute boucle doit avoir une borne supérieure fixe pour garantir la terminaison et calculer le WCET.
