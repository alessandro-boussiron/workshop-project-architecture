# Règle 4 : Limiter la Taille des Fonctions

## 📋 Énoncé

**Les fonctions doivent tenir sur une page imprimée**
- Maximum **60 lignes** de code par fonction
- Une fonction = une responsabilité
- Faciliter la lecture et la vérification

## 🎯 Objectif

- **Lisibilité**: Comprendre une fonction d'un seul coup d'œil
- **Testabilité**: Fonctions simples = tests simples
- **Maintenabilité**: Petites fonctions = modifications localisées
- **Vérifiabilité**: Code review plus efficace

## ❌ Problèmes

### Fonction géante (100+ lignes)

```c
// ❌ MAUVAIS - Fonction monstre
void bad_process_telemetry(uint8_t *data, size_t size) {
    // Validation (10 lignes)
    if (data == NULL) return;
    if (size < 10) return;
    
    // Parsing (20 lignes)
    uint16_t packet_id = (data[0] << 8) | data[1];
    uint8_t command = data[2];
    // ... beaucoup de parsing ...
    
    // Traitement (30 lignes)
    switch (command) {
        case CMD_READ:
            // 10 lignes de code
            break;
        case CMD_WRITE:
            // 10 lignes de code
            break;
        case CMD_CONFIG:
            // 10 lignes de code
            break;
    }
    
    // Validation post-traitement (20 lignes)
    // ... calculs de checksum ...
    // ... vérifications ...
    
    // Logging (10 lignes)
    // ... logs détaillés ...
    
    // Statistiques (10 lignes)
    // ... mise à jour stats ...
}
// Total: 100+ lignes = MAUVAIS!
```

**Problèmes:**
- Impossible à comprendre d'un coup
- Difficile à tester
- Bugs cachés dans la complexité
- Modifications risquées

## ✅ Solutions

### Décomposition en fonctions

```c
// ✅ BON - Fonctions focalisées

// 5 lignes
bool validate_packet(const uint8_t *data, size_t size) {
    return (data != NULL && size >= MIN_PACKET_SIZE);
}

// 8 lignes
PacketHeader parse_header(const uint8_t *data) {
    PacketHeader header;
    header.packet_id = (data[0] << 8) | data[1];
    header.command = data[2];
    header.length = data[3];
    header.checksum = data[4];
    return header;
}

// 15 lignes
bool process_read_command(const PacketHeader *header, const uint8_t *payload) {
    assert(header != NULL && payload != NULL);
    
    uint16_t address = (payload[0] << 8) | payload[1];
    uint8_t length = payload[2];
    
    if (address > MAX_ADDRESS || length > MAX_READ_LENGTH) {
        return false;
    }
    
    read_memory(address, length);
    return true;
}

// 12 lignes
bool verify_checksum(const uint8_t *data, size_t size, uint8_t expected) {
    uint8_t checksum = 0;
    
    for (size_t i = 0; i < size; i++) {
        checksum ^= data[i];
    }
    
    return (checksum == expected);
}

// 10 lignes
void update_statistics(const PacketHeader *header, bool success) {
    g_stats.total_packets++;
    
    if (success) {
        g_stats.successful_packets++;
    } else {
        g_stats.failed_packets++;
    }
    
    log_packet(header, success);
}

// 20 lignes - Fonction principale (orchestration)
void good_process_telemetry(uint8_t *data, size_t size) {
    if (!validate_packet(data, size)) {
        update_statistics(NULL, false);
        return;
    }
    
    PacketHeader header = parse_header(data);
    
    if (!verify_checksum(data, size - 1, header.checksum)) {
        update_statistics(&header, false);
        return;
    }
    
    bool success = false;
    const uint8_t *payload = data + HEADER_SIZE;
    
    switch (header.command) {
        case CMD_READ:
            success = process_read_command(&header, payload);
            break;
        // ... autres commandes ...
    }
    
    update_statistics(&header, success);
}
```

## 📊 Principes de Décomposition

### 1. Single Responsibility

```c
// ❌ MAUVAIS - Fait trop de choses
void bad_handle_sensor(int sensor_id) {
    int raw_value = read_adc(sensor_id);
    float voltage = raw_value * 3.3 / 4096.0;
    float temperature = (voltage - 0.5) * 100.0;
    
    if (temperature > 80.0) {
        trigger_alarm();
        log_event("Overheat");
        send_notification();
    }
    
    store_to_database(temperature);
    update_display(temperature);
}

// ✅ BON - Responsabilités séparées
float read_sensor_temperature(int sensor_id) {
    int raw_value = read_adc(sensor_id);
    float voltage = adc_to_voltage(raw_value);
    return voltage_to_temperature(voltage);
}

void check_temperature_alarm(float temperature) {
    if (temperature > THRESHOLD_TEMP) {
        trigger_alarm();
        log_overheat_event();
        send_overheat_notification();
    }
}

void record_temperature(float temperature) {
    store_to_database(temperature);
    update_display(temperature);
}

void handle_sensor(int sensor_id) {
    float temp = read_sensor_temperature(sensor_id);
    check_temperature_alarm(temp);
    record_temperature(temp);
}
```

### 2. Extraction de Helpers

```c
// ✅ BON - Helpers pour calculs

static inline float adc_to_voltage(int adc_value) {
    return adc_value * (3.3f / 4096.0f);
}

static inline float voltage_to_temperature(float voltage) {
    return (voltage - 0.5f) * 100.0f;
}

static inline bool is_temperature_critical(float temp) {
    return (temp > CRITICAL_THRESHOLD);
}
```

### 3. Tables de Dispatch

```c
// ✅ BON - Table au lieu de gros switch

typedef bool (*CommandHandler)(const uint8_t *payload);

typedef struct {
    uint8_t command_id;
    CommandHandler handler;
} CommandDispatch;

static const CommandDispatch command_table[] = {
    {CMD_READ,   handle_read_command},
    {CMD_WRITE,  handle_write_command},
    {CMD_CONFIG, handle_config_command},
    {CMD_STATUS, handle_status_command},
};

bool dispatch_command(uint8_t command_id, const uint8_t *payload) {
    for (size_t i = 0; i < ARRAY_SIZE(command_table); i++) {
        if (command_table[i].command_id == command_id) {
            return command_table[i].handler(payload);
        }
    }
    return false;  // Unknown command
}
```

## 🔍 Mesure de Complexité

### Cyclomatic Complexity

```c
// Complexité = nombre de chemins d'exécution

// Simple: complexité = 1
void simple_function(int x) {
    print(x);
}

// Moyenne: complexité = 3
int moderate_function(int x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

// Élevée: complexité = 8
int complex_function(int x, int y) {
    if (x > 0) {
        if (y > 0) {
            return 1;
        } else {
            return 2;
        }
    } else if (x < 0) {
        if (y > 0) {
            return 3;
        } else {
            return 4;
        }
    } else {
        if (y > 0) {
            return 5;
        } else if (y < 0) {
            return 6;
        } else {
            return 0;
        }
    }
}
```

**Objectif**: Complexité ≤ 10

## 🛠️ Outils

### Analyse automatique

```bash
# Lizard (Cyclomatic Complexity)
lizard -l c *.c

# CCCC (Code metrics)
cccc *.c

# SLOCCount (lignes de code)
sloccount .

# Clang metrics
clang-tidy -checks='readability-function-size' *.c
```

### Scripts personnalisés

```bash
# Compter lignes par fonction
awk '/^[a-zA-Z].*\(.*\).*{$/{name=$1; count=0} 
     {count++} 
     /^}$/{if(count>60) print name, count}' *.c
```

## 📚 Patterns

### State Machine

```c
// ✅ BON - Chaque état = petite fonction

typedef enum {
    STATE_IDLE,
    STATE_READING,
    STATE_PROCESSING,
    STATE_SENDING
} State;

State handle_idle_state(void) {
    if (data_available()) {
        return STATE_READING;
    }
    return STATE_IDLE;
}

State handle_reading_state(void) {
    if (read_data()) {
        return STATE_PROCESSING;
    }
    return STATE_IDLE;
}

State handle_processing_state(void) {
    if (process_data()) {
        return STATE_SENDING;
    }
    return STATE_IDLE;
}

State handle_sending_state(void) {
    send_data();
    return STATE_IDLE;
}

void state_machine_run(void) {
    State current_state = STATE_IDLE;
    
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        switch (current_state) {
            case STATE_IDLE:
                current_state = handle_idle_state();
                break;
            case STATE_READING:
                current_state = handle_reading_state();
                break;
            case STATE_PROCESSING:
                current_state = handle_processing_state();
                break;
            case STATE_SENDING:
                current_state = handle_sending_state();
                break;
        }
    }
}
```

## 💡 Points clés

1. **Maximum 60 lignes**: Tenant sur une page
2. **Une responsabilité**: Fonction fait UNE chose
3. **Niveaux d'abstraction**: Haute abstraction en haut, détails en bas
4. **Noms descriptifs**: Nom dit ce que fait la fonction
5. **Peu de paramètres**: Maximum 3-4 paramètres

## 🎓 Exercices

Voir `exercises/ex04_function_size.c`

---

**Résumé**: Fonctions < 60 lignes, une responsabilité par fonction, décomposer en helpers.
