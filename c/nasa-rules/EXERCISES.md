# NASA Power of 10 Rules - Exercises

Exercices pratiques pour maîtriser les règles NASA.

## 🎯 Exercise 1: Control Flow Refactoring

**Fichier**: `exercises/ex01_control_flow.c`

**Objectif**: Corriger le code suivant pour respecter la règle 1.

```c
// ❌ Code à corriger
int process_commands(char **commands, int count) {
    int i = 0;
    
start:
    if (i >= count) goto end;
    
    if (strcmp(commands[i], "START") == 0) {
        printf("Starting...\n");
        i++;
        goto start;
    }
    
    if (strcmp(commands[i], "STOP") == 0) {
        printf("Stopping...\n");
        goto end;
    }
    
    i++;
    goto start;
    
end:
    return i;
}
```

**Tâches**:
1. Éliminer tous les `goto`
2. Utiliser une structure de contrôle claire (for/while/switch)
3. Ajouter gestion d'erreurs propre
4. Compiler sans warnings

**Solution attendue**: Boucle `for` avec `switch` pour les commandes.

---

## 🔄 Exercise 2: Loop Bounds

**Fichier**: `exercises/ex02_loop_bounds.c`

**Objectif**: Ajouter des bornes fixes à toutes les boucles.

```c
// ❌ Code à corriger
void parse_input(char *input) {
    int i = 0;
    while (input[i] != '\0') {  // Unbounded!
        process_char(input[i]);
        i++;
    }
}

int find_value(int *array, int value) {
    int i = 0;
    while (array[i] != value) {  // May never terminate!
        i++;
    }
    return i;
}

void wait_for_ready(volatile bool *ready) {
    while (!*ready) {  // Infinite wait!
        // Wait
    }
}
```

**Tâches**:
1. Ajouter `#define MAX_INPUT_SIZE 256`
2. Ajouter `#define MAX_ARRAY_SIZE 100`
3. Ajouter `#define MAX_WAIT_CYCLES 1000`
4. Borner toutes les boucles
5. Retourner codes d'erreur appropriés

---

## 💾 Exercise 3: Static Allocation

**Fichier**: `exercises/ex03_static_memory.c`

**Objectif**: Convertir allocation dynamique en statique.

```c
// ❌ Code à corriger
typedef struct {
    char *name;
    int *data;
    size_t size;
} Sensor;

Sensor* create_sensor(const char *name, size_t data_size) {
    Sensor *sensor = malloc(sizeof(Sensor));
    sensor->name = malloc(strlen(name) + 1);
    strcpy(sensor->name, name);
    sensor->data = malloc(data_size * sizeof(int));
    sensor->size = data_size;
    return sensor;
}

void destroy_sensor(Sensor *sensor) {
    free(sensor->name);
    free(sensor->data);
    free(sensor);
}
```

**Tâches**:
1. Créer pool de capteurs statique (`MAX_SENSORS = 10`)
2. Utiliser buffers fixes pour noms et données
3. Implémenter `sensor_acquire()` et `sensor_release()`
4. Plus aucun `malloc/free`

---

## 📏 Exercise 4: Function Size

**Fichier**: `exercises/ex04_function_size.c`

**Objectif**: Décomposer fonction trop longue.

```c
// ❌ Fonction > 60 lignes
void process_telemetry_packet(uint8_t *packet, size_t len) {
    // Validate header (10 lines)
    if (packet[0] != 0xAA || packet[1] != 0x55) {
        log_error("Invalid header");
        return;
    }
    
    // Parse fields (15 lines)
    uint16_t sensor_id = (packet[2] << 8) | packet[3];
    float temperature = /* complex calculation */ 0;
    float pressure = /* complex calculation */ 0;
    
    // Validate ranges (20 lines)
    if (temperature < -50 || temperature > 100) {
        log_error("Temperature out of range");
        return;
    }
    // ... more validation
    
    // Store in buffer (15 lines)
    // ... buffering logic
    
    // Update statistics (15 lines)
    // ... stats update
    
    // Trigger alerts if needed (10 lines)
    // ... alert logic
}
```

**Tâches**:
1. Découper en fonctions < 15 lignes chacune
2. `validate_packet_header()`
3. `parse_telemetry_fields()`
4. `validate_sensor_ranges()`
5. `store_telemetry_sample()`
6. `update_statistics()`
7. `check_alert_conditions()`

---

## ✅ Exercise 5: Return Value Checking

**Fichier**: `exercises/ex05_return_checks.c`

**Objectif**: Ajouter vérifications de toutes les valeurs de retour.

```c
// ❌ Code à corriger
void load_configuration(const char *filename) {
    FILE *file = fopen(filename, "r");  // Pas de vérification
    
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);  // Pas de vérification
    
    int config_value = atoi(buffer);  // Pas de vérification
    
    fclose(file);  // Pas de vérification
    
    apply_config(config_value);  // Pas de vérification
}
```

**Tâches**:
1. Vérifier chaque appel de fonction
2. Créer enum pour codes d'erreur
3. Retourner statut approprié
4. Logger les erreurs
5. Cleanup propre en cas d'erreur

---

## 🔍 Exercise 6: Variable Scope

**Fichier**: `exercises/ex06_variable_scope.c`

**Objectif**: Réduire le scope des variables au minimum.

```c
// ❌ Code à corriger
void process_data(void) {
    int i, j, k;  // Scope trop large
    int temp, sum, count;
    char buffer[256];
    bool found;
    
    // Use i here
    for (i = 0; i < 10; i++) {
        sum += i;
    }
    
    // Use j here (different context)
    for (j = 0; j < 20; j++) {
        count++;
    }
    
    // Use buffer here
    sprintf(buffer, "Result: %d", sum);
    
    // found never used
}
```

**Tâches**:
1. Déclarer variables au plus petit scope
2. Supprimer variables non utilisées
3. Variables loop dans le for
4. Séparer en fonctions si nécessaire

---

## 🎓 Exercise 7: Comprehensive System

**Fichier**: `exercises/ex07_complete_system.c`

**Objectif**: Créer système complet respectant toutes les règles.

**Spécification**: Système de monitoring température avec:
- 4 capteurs (température, pression, humidité, voltage)
- Buffer circulaire de 100 échantillons
- Calcul moyenne/min/max
- Détection d'alertes (seuils configurables)
- Log des événements
- Interface commande (START/STOP/STATUS/RESET)

**Contraintes**:
- ✅ Pas de goto
- ✅ Toutes boucles bornées
- ✅ Pas de malloc/free
- ✅ Fonctions < 60 lignes
- ✅ Vérifier tous les returns
- ✅ Scope minimal
- ✅ Assertions partout
- ✅ Max 2 niveaux pointeurs
- ✅ Compiler avec `-Wall -Wextra -Werror`

---

## 🏆 Exercise 8: Code Review Challenge

**Fichier**: Fourni dans `exercises/ex08_review.c`

**Objectif**: Trouver toutes les violations des 10 règles.

Le fichier contient intentionnellement 20+ violations. Pour chaque violation:
1. Identifier la règle violée
2. Expliquer pourquoi c'est problématique
3. Proposer correction
4. Implémenter la correction

**Exemple de format de réponse**:
```
Ligne 42: while(1) { ... }
Règle violée: Rule 2 (Fixed Loop Bounds)
Problème: Boucle sans borne fixe visible
Correction: Ajouter compteur avec MAX_ITERATIONS
```

---

## 📊 Grading Rubric

| Critère | Points | Description |
|---------|--------|-------------|
| Compilation | 20 | `-Wall -Wextra -Werror` sans warnings |
| Rule 1 | 10 | Pas de goto/recursion |
| Rule 2 | 10 | Toutes boucles bornées |
| Rule 3 | 10 | Pas de malloc/free runtime |
| Rule 4 | 10 | Fonctions < 60 lignes |
| Rule 5 | 10 | Returns vérifiés |
| Rule 6 | 5 | Scope minimal |
| Rule 7 | 5 | Assertions |
| Rule 8 | 5 | Max 2 niveaux pointeurs |
| Rule 9 | 10 | Zero warnings |
| Rule 10 | 5 | Passe static analysis |
| **Total** | **100** | |

---

## 💡 Tips pour réussir

1. **Commencer simple**: Exercise 1-3 d'abord
2. **Compiler souvent**: Après chaque changement
3. **Lire les exemples**: Les fichiers `rule0X_*.c` contiennent les patterns
4. **Utiliser les outils**:
   ```bash
   gcc -Wall -Wextra -Werror -pedantic -std=c11 file.c
   clang --analyze file.c
   cppcheck file.c
   ```
5. **Tester**: Créer des cas de test pour chaque fonction

---

## 📚 Resources

- `rule01_control_flow.c` - Exemples règle 1
- `rule02_loop_bounds.c` - Exemples règle 2
- `rule03_no_dynamic_memory.c` - Exemples règle 3
- `nasa_rules.c` - Vue d'ensemble complète
- NASA JPL Coding Standard document
