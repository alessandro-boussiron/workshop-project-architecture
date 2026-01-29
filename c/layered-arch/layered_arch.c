/*
 * LAYERED ARCHITECTURE IN C
 * 
 * Example: Temperature Monitoring System
 * Demonstrates clean separation of concerns in embedded systems
 * 
 * Compilation: gcc -Wall -Wextra -Werror -std=c11 layered_arch.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>

// ============================================
// LAYER 1: HARDWARE ABSTRACTION LAYER (HAL)
// Lowest level - hardware interface
// ============================================

/* HAL - SPI Interface (abstract) */
typedef struct {
    bool (*init)(void);
    bool (*transfer)(uint8_t *tx_data, uint8_t *rx_data, size_t len);
    void (*deinit)(void);
} SpiInterface;

/* HAL - GPIO Interface (abstract) */
typedef struct {
    bool (*set_pin)(uint8_t pin, bool state);
    bool (*get_pin)(uint8_t pin);
} GpioInterface;

/* HAL - UART Interface (abstract) */
typedef struct {
    bool (*init)(uint32_t baudrate);
    size_t (*write)(const uint8_t *data, size_t len);
    size_t (*read)(uint8_t *data, size_t len);
} UartInterface;

// ============================================
// LAYER 1: HAL IMPLEMENTATIONS (Hardware-specific)
// ============================================

/* Mock SPI Implementation */
static bool mock_spi_init(void) {
    printf("  [HAL] SPI initialized\n");
    return true;
}

static bool mock_spi_transfer(uint8_t *tx_data, uint8_t *rx_data, size_t len) {
    printf("  [HAL] SPI transfer: %zu bytes\n", len);
    
    // Simulate temperature sensor response
    if (rx_data != NULL && len >= 2) {
        rx_data[0] = 0x19;  // 25°C high byte
        rx_data[1] = 0x00;  // Low byte
    }
    
    return true;
}

static void mock_spi_deinit(void) {
    printf("  [HAL] SPI deinitialized\n");
}

static const SpiInterface mock_spi = {
    .init = mock_spi_init,
    .transfer = mock_spi_transfer,
    .deinit = mock_spi_deinit
};

/* Mock GPIO Implementation */
static bool mock_gpio_set(uint8_t pin, bool state) {
    printf("  [HAL] GPIO pin %d set to %d\n", pin, state);
    return true;
}

static bool mock_gpio_get(uint8_t pin) {
    printf("  [HAL] GPIO pin %d read\n", pin);
    return true;
}

static const GpioInterface mock_gpio = {
    .set_pin = mock_gpio_set,
    .get_pin = mock_gpio_get
};

/* Mock UART Implementation */
static bool mock_uart_init(uint32_t baudrate) {
    printf("  [HAL] UART initialized at %u baud\n", baudrate);
    return true;
}

static size_t mock_uart_write(const uint8_t *data, size_t len) {
    printf("  [HAL] UART write: %.*s", (int)len, (char*)data);
    return len;
}

static size_t mock_uart_read(uint8_t *data, size_t len) {
    (void)data;
    printf("  [HAL] UART read: %zu bytes\n", len);
    return 0;
}

static const UartInterface mock_uart = {
    .init = mock_uart_init,
    .write = mock_uart_write,
    .read = mock_uart_read
};

// ============================================
// LAYER 2: DRIVER LAYER
// Device drivers using HAL
// ============================================

/* Temperature Sensor Driver (uses SPI) */
typedef struct {
    const SpiInterface *spi;
    uint8_t cs_pin;
    bool initialized;
} TempSensorDriver;

bool temp_sensor_init(TempSensorDriver *driver, const SpiInterface *spi, uint8_t cs_pin) {
    assert(driver != NULL);
    assert(spi != NULL);
    
    driver->spi = spi;
    driver->cs_pin = cs_pin;
    
    if (!spi->init()) {
        return false;
    }
    
    driver->initialized = true;
    printf("  [DRIVER] Temperature sensor initialized\n");
    return true;
}

bool temp_sensor_read(TempSensorDriver *driver, float *temperature) {
    assert(driver != NULL);
    assert(temperature != NULL);
    
    if (!driver->initialized) {
        return false;
    }
    
    uint8_t tx_data[2] = {0x00, 0x00};
    uint8_t rx_data[2] = {0x00, 0x00};
    
    // Read temperature register
    if (!driver->spi->transfer(tx_data, rx_data, 2)) {
        return false;
    }
    
    // Convert raw data to temperature
    int16_t raw = (int16_t)((rx_data[0] << 8) | rx_data[1]);
    *temperature = raw / 10.0f;
    
    printf("  [DRIVER] Temperature read: %.1f°C\n", *temperature);
    return true;
}

void temp_sensor_deinit(TempSensorDriver *driver) {
    if (driver != NULL && driver->initialized) {
        driver->spi->deinit();
        driver->initialized = false;
        printf("  [DRIVER] Temperature sensor deinitialized\n");
    }
}

/* LED Driver (uses GPIO) */
typedef struct {
    const GpioInterface *gpio;
    uint8_t pin;
    bool state;
} LedDriver;

void led_init(LedDriver *led, const GpioInterface *gpio, uint8_t pin) {
    assert(led != NULL);
    assert(gpio != NULL);
    
    led->gpio = gpio;
    led->pin = pin;
    led->state = false;
    
    gpio->set_pin(pin, false);
    printf("  [DRIVER] LED initialized on pin %d\n", pin);
}

void led_on(LedDriver *led) {
    assert(led != NULL);
    led->gpio->set_pin(led->pin, true);
    led->state = true;
    printf("  [DRIVER] LED ON\n");
}

void led_off(LedDriver *led) {
    assert(led != NULL);
    led->gpio->set_pin(led->pin, false);
    led->state = false;
    printf("  [DRIVER] LED OFF\n");
}

void led_toggle(LedDriver *led) {
    assert(led != NULL);
    led->state = !led->state;
    led->gpio->set_pin(led->pin, led->state);
    printf("  [DRIVER] LED toggled to %s\n", led->state ? "ON" : "OFF");
}

/* Logger Driver (uses UART) */
typedef struct {
    const UartInterface *uart;
    bool initialized;
} LoggerDriver;

bool logger_init(LoggerDriver *logger, const UartInterface *uart) {
    assert(logger != NULL);
    assert(uart != NULL);
    
    logger->uart = uart;
    
    if (!uart->init(115200)) {
        return false;
    }
    
    logger->initialized = true;
    printf("  [DRIVER] Logger initialized\n");
    return true;
}

void logger_log(LoggerDriver *logger, const char *message) {
    assert(logger != NULL);
    assert(message != NULL);
    
    if (!logger->initialized) {
        return;
    }
    
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer), "[LOG] %s\n", message);
    
    if (len > 0 && (size_t)len < sizeof(buffer)) {
        logger->uart->write((uint8_t*)buffer, (size_t)len);
    }
}

// ============================================
// LAYER 3: SERVICE LAYER
// Business logic coordination
// ============================================

typedef enum {
    TEMP_STATUS_NORMAL,
    TEMP_STATUS_WARNING,
    TEMP_STATUS_CRITICAL
} TempStatus;

typedef struct {
    float current_temp;
    float min_temp;
    float max_temp;
    float warning_threshold;
    float critical_threshold;
    TempStatus status;
    uint32_t reading_count;
} TempMonitorService;

void temp_monitor_init(TempMonitorService *service,
                       float warning_threshold,
                       float critical_threshold) {
    assert(service != NULL);
    
    memset(service, 0, sizeof(TempMonitorService));
    service->warning_threshold = warning_threshold;
    service->critical_threshold = critical_threshold;
    service->min_temp = 1000.0f;
    service->max_temp = -1000.0f;
    service->status = TEMP_STATUS_NORMAL;
    
    printf("  [SERVICE] Temperature monitor initialized\n");
    printf("    Warning: %.1f°C, Critical: %.1f°C\n",
           warning_threshold, critical_threshold);
}

TempStatus temp_monitor_process(TempMonitorService *service, float temperature) {
    assert(service != NULL);
    
    service->current_temp = temperature;
    service->reading_count++;
    
    // Update min/max
    if (temperature < service->min_temp) {
        service->min_temp = temperature;
    }
    if (temperature > service->max_temp) {
        service->max_temp = temperature;
    }
    
    // Determine status
    if (temperature >= service->critical_threshold) {
        service->status = TEMP_STATUS_CRITICAL;
    } else if (temperature >= service->warning_threshold) {
        service->status = TEMP_STATUS_WARNING;
    } else {
        service->status = TEMP_STATUS_NORMAL;
    }
    
    printf("  [SERVICE] Temp: %.1f°C, Status: ", temperature);
    switch (service->status) {
        case TEMP_STATUS_NORMAL:   printf("NORMAL\n"); break;
        case TEMP_STATUS_WARNING:  printf("WARNING\n"); break;
        case TEMP_STATUS_CRITICAL: printf("CRITICAL\n"); break;
    }
    
    return service->status;
}

void temp_monitor_get_stats(const TempMonitorService *service,
                            float *avg_temp,
                            float *min_temp,
                            float *max_temp) {
    assert(service != NULL);
    
    if (avg_temp != NULL) {
        *avg_temp = service->current_temp;  // Simplified
    }
    if (min_temp != NULL) {
        *min_temp = service->min_temp;
    }
    if (max_temp != NULL) {
        *max_temp = service->max_temp;
    }
}

// ============================================
// LAYER 4: APPLICATION LAYER
// High-level application logic
// ============================================

typedef struct {
    TempSensorDriver temp_sensor;
    LedDriver status_led;
    LoggerDriver logger;
    TempMonitorService monitor;
    bool running;
} Application;

bool app_init(Application *app) {
    assert(app != NULL);
    
    printf("\n[APP] Initializing application...\n");
    
    // Initialize drivers
    if (!temp_sensor_init(&app->temp_sensor, &mock_spi, 10)) {
        printf("[APP] Failed to initialize temperature sensor\n");
        return false;
    }
    
    led_init(&app->status_led, &mock_gpio, 5);
    
    if (!logger_init(&app->logger, &mock_uart)) {
        printf("[APP] Failed to initialize logger\n");
        return false;
    }
    
    // Initialize service
    temp_monitor_init(&app->monitor, 30.0f, 40.0f);
    
    app->running = true;
    
    logger_log(&app->logger, "Application initialized successfully");
    printf("[APP] ✅ Initialization complete\n\n");
    
    return true;
}

void app_run_cycle(Application *app) {
    assert(app != NULL);
    
    if (!app->running) {
        return;
    }
    
    printf("\n[APP] === Running cycle ===\n");
    
    // Read temperature
    float temperature;
    if (!temp_sensor_read(&app->temp_sensor, &temperature)) {
        logger_log(&app->logger, "ERROR: Failed to read temperature");
        led_on(&app->status_led);  // Error indication
        return;
    }
    
    // Process temperature
    TempStatus status = temp_monitor_process(&app->monitor, temperature);
    
    // Update LED based on status
    switch (status) {
        case TEMP_STATUS_NORMAL:
            led_off(&app->status_led);
            logger_log(&app->logger, "Temperature normal");
            break;
            
        case TEMP_STATUS_WARNING:
            led_toggle(&app->status_led);  // Blink
            logger_log(&app->logger, "WARNING: Temperature high");
            break;
            
        case TEMP_STATUS_CRITICAL:
            led_on(&app->status_led);
            logger_log(&app->logger, "CRITICAL: Temperature too high!");
            break;
    }
}

void app_print_stats(const Application *app) {
    assert(app != NULL);
    
    float min_temp, max_temp, avg_temp;
    temp_monitor_get_stats(&app->monitor, &avg_temp, &min_temp, &max_temp);
    
    printf("\n[APP] === Statistics ===\n");
    printf("  Readings: %u\n", app->monitor.reading_count);
    printf("  Current: %.1f°C\n", app->monitor.current_temp);
    printf("  Min: %.1f°C\n", min_temp);
    printf("  Max: %.1f°C\n", max_temp);
    printf("  Status: ");
    
    switch (app->monitor.status) {
        case TEMP_STATUS_NORMAL:   printf("NORMAL\n"); break;
        case TEMP_STATUS_WARNING:  printf("WARNING\n"); break;
        case TEMP_STATUS_CRITICAL: printf("CRITICAL\n"); break;
    }
}

void app_shutdown(Application *app) {
    if (app == NULL) {
        return;
    }
    
    printf("\n[APP] Shutting down...\n");
    
    logger_log(&app->logger, "Application shutting down");
    
    led_off(&app->status_led);
    temp_sensor_deinit(&app->temp_sensor);
    
    app->running = false;
    
    printf("[APP] ✅ Shutdown complete\n");
}

// ============================================
// MAIN - System Entry Point
// ============================================

int main(void) {
    printf("🏗️  LAYERED ARCHITECTURE IN C\n");
    printf("Temperature Monitoring System\n");
    printf("================================\n");
    
    Application app = {0};
    
    // Initialize application
    if (!app_init(&app)) {
        fprintf(stderr, "Failed to initialize application\n");
        return EXIT_FAILURE;
    }
    
    // Simulate multiple cycles
    for (int i = 0; i < 3; i++) {
        app_run_cycle(&app);
        
        // Simulate different conditions by modifying mock data
        // In real system, this would be actual sensor readings
    }
    
    // Print statistics
    app_print_stats(&app);
    
    // Shutdown
    app_shutdown(&app);
    
    printf("\n🎉 Application complete!\n");
    
    return EXIT_SUCCESS;
}

/*
 * ============================================
 * LAYERED ARCHITECTURE BENEFITS
 * ============================================
 *
 * ✅ PORTABILITY
 * - Change hardware by swapping HAL implementation
 * - Application code remains unchanged
 * - Easy to support multiple platforms
 *
 * ✅ TESTABILITY
 * - Mock HAL for unit testing
 * - Test application without hardware
 * - Automated testing possible
 *
 * ✅ MAINTAINABILITY
 * - Clear separation of concerns
 * - Changes isolated to specific layers
 * - Easy to understand and modify
 *
 * ✅ REUSABILITY
 * - Drivers reusable across projects
 * - Services independent of hardware
 * - HAL abstractions standardized
 *
 * LAYER RESPONSIBILITIES:
 *
 * 1. HAL (Hardware Abstraction Layer)
 *    - Direct hardware access
 *    - Register manipulation
 *    - Platform-specific code
 *
 * 2. DRIVER LAYER
 *    - Device-specific logic
 *    - Uses HAL interfaces
 *    - Hardware-independent API
 *
 * 3. SERVICE LAYER
 *    - Business logic
 *    - Coordinates multiple drivers
 *    - Application-specific algorithms
 *
 * 4. APPLICATION LAYER
 *    - High-level orchestration
 *    - User-facing functionality
 *    - System behavior
 *
 * DEPENDENCY RULE:
 * - Higher layers depend on lower layers
 * - Lower layers don't know about higher layers
 * - Use interfaces for flexibility
 *
 * SIMILAR TO:
 * - Clean Architecture (web)
 * - Hexagonal Architecture
 * - Onion Architecture
 */
