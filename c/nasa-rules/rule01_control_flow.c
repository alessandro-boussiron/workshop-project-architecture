/*
 * NASA RULE 1: RESTRICT CONTROL FLOW
 * 
 * No goto, setjmp/longjmp, or indirect recursion
 * Keep control flow simple and predictable
 * 
 * Compilation: gcc -Wall -Wextra -Werror -std=c11 rule01_control_flow.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// ============================================
// ❌ BAD EXAMPLES - What NOT to do
// ============================================

/* BAD: Using goto for error handling (confusing flow) */
int bad_process_data_goto(int *data, size_t size) {
    int result = 0;
    
    if (data == NULL)
        goto error;
    
    if (size == 0)
        goto error;
    
    for (size_t i = 0; i < size; i++) {
        if (data[i] < 0)
            goto error;
        result += data[i];
    }
    
    return result;

error:
    return -1;
}

/* BAD: Multiple gotos creating spaghetti code */
int bad_state_machine_goto(int state) {
    int counter = 0;
    
start:
    if (state == 0) {
        counter++;
        state = 1;
        goto state1;
    }
    
state1:
    if (state == 1) {
        counter += 10;
        state = 2;
        goto state2;
    }
    
state2:
    if (state == 2) {
        counter += 100;
        if (counter < 200)
            goto start;  // Jump backwards!
    }
    
    return counter;
}

/* BAD: Direct recursion (unbounded) */
int bad_factorial_recursive(int n) {
    if (n <= 1)
        return 1;
    return n * bad_factorial_recursive(n - 1);  // Can stack overflow
}

/* BAD: Indirect recursion */
void bad_func_a(int n);
void bad_func_b(int n);

void bad_func_a(int n) {
    if (n > 0) {
        printf("A: %d\n", n);
        bad_func_b(n - 1);
    }
}

void bad_func_b(int n) {
    if (n > 0) {
        printf("B: %d\n", n);
        bad_func_a(n - 1);  // Indirect recursion
    }
}

// ============================================
// ✅ GOOD EXAMPLES - Proper control flow
// ============================================

/* GOOD: Structured error handling with early returns */
int good_process_data(const int *data, size_t size) {
    if (data == NULL) {
        return -1;
    }
    
    if (size == 0) {
        return -1;
    }
    
    int result = 0;
    for (size_t i = 0; i < size; i++) {
        if (data[i] < 0) {
            return -1;
        }
        result += data[i];
    }
    
    return result;
}

/* GOOD: State machine with switch statement */
typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_ERROR
} State;

int good_state_machine(State state, int input) {
    int output = 0;
    
    switch (state) {
        case STATE_IDLE:
            output = input;
            break;
            
        case STATE_RUNNING:
            output = input * 2;
            break;
            
        case STATE_PAUSED:
            output = 0;
            break;
            
        case STATE_ERROR:
            output = -1;
            break;
            
        default:
            output = -1;
            break;
    }
    
    return output;
}

/* GOOD: Iterative factorial (bounded) */
#define MAX_FACTORIAL_INPUT 20

int good_factorial_iterative(int n) {
    if (n < 0 || n > MAX_FACTORIAL_INPUT) {
        return -1;  // Error
    }
    
    if (n <= 1) {
        return 1;
    }
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    
    return result;
}

/* GOOD: Replace indirect recursion with iteration */
#define MAX_ITERATIONS 100

void good_alternating_process(int n) {
    if (n <= 0 || n > MAX_ITERATIONS) {
        return;
    }
    
    for (int i = n; i > 0; i--) {
        if (i % 2 == 0) {
            printf("Even: %d\n", i);
        } else {
            printf("Odd: %d\n", i);
        }
    }
}

/* GOOD: Complex logic with clear structure */
typedef enum {
    ERROR_NONE = 0,
    ERROR_NULL_POINTER,
    ERROR_INVALID_SIZE,
    ERROR_INVALID_DATA
} ErrorCode;

ErrorCode good_validate_and_process(const int *data, size_t size, int *output) {
    // Validate inputs
    if (data == NULL || output == NULL) {
        return ERROR_NULL_POINTER;
    }
    
    if (size == 0 || size > 1000) {
        return ERROR_INVALID_SIZE;
    }
    
    // Process data
    int sum = 0;
    for (size_t i = 0; i < size; i++) {
        if (data[i] < 0 || data[i] > 1000) {
            return ERROR_INVALID_DATA;
        }
        sum += data[i];
    }
    
    *output = sum;
    return ERROR_NONE;
}

// ============================================
// REAL-WORLD EXAMPLES
// ============================================

/* Example: Command parser with structured flow */
typedef enum {
    CMD_START,
    CMD_STOP,
    CMD_RESET,
    CMD_STATUS,
    CMD_UNKNOWN
} Command;

Command parse_command(const char *cmd_string) {
    if (cmd_string == NULL) {
        return CMD_UNKNOWN;
    }
    
    // Simple string comparison (in real code, use strncmp)
    if (cmd_string[0] == 'S' && cmd_string[1] == 'T' && cmd_string[2] == 'A') {
        if (cmd_string[3] == 'R' && cmd_string[4] == 'T') {
            return CMD_START;
        }
        if (cmd_string[3] == 'T' && cmd_string[4] == 'U') {
            return CMD_STATUS;
        }
    }
    
    if (cmd_string[0] == 'S' && cmd_string[1] == 'T' && cmd_string[2] == 'O') {
        return CMD_STOP;
    }
    
    if (cmd_string[0] == 'R' && cmd_string[1] == 'E' && cmd_string[2] == 'S') {
        return CMD_RESET;
    }
    
    return CMD_UNKNOWN;
}

bool execute_command(Command cmd) {
    switch (cmd) {
        case CMD_START:
            printf("System starting...\n");
            return true;
            
        case CMD_STOP:
            printf("System stopping...\n");
            return true;
            
        case CMD_RESET:
            printf("System resetting...\n");
            return true;
            
        case CMD_STATUS:
            printf("System status: OK\n");
            return true;
            
        case CMD_UNKNOWN:
            printf("Unknown command\n");
            return false;
            
        default:
            return false;
    }
}

/* Example: Packet processing with clear flow */
typedef struct {
    uint8_t header;
    uint8_t type;
    uint16_t length;
    uint8_t data[256];
    uint8_t checksum;
} Packet;

bool process_packet(const Packet *packet) {
    // Validation phase
    if (packet == NULL) {
        return false;
    }
    
    if (packet->header != 0xAA) {
        printf("Invalid header\n");
        return false;
    }
    
    if (packet->length > 256) {
        printf("Invalid length\n");
        return false;
    }
    
    // Calculate checksum
    uint8_t calculated_checksum = 0;
    for (uint16_t i = 0; i < packet->length; i++) {
        calculated_checksum ^= packet->data[i];
    }
    
    if (calculated_checksum != packet->checksum) {
        printf("Checksum mismatch\n");
        return false;
    }
    
    // Process based on type
    switch (packet->type) {
        case 0x01:
            printf("Processing data packet\n");
            break;
            
        case 0x02:
            printf("Processing control packet\n");
            break;
            
        case 0x03:
            printf("Processing status packet\n");
            break;
            
        default:
            printf("Unknown packet type\n");
            return false;
    }
    
    return true;
}

// ============================================
// MAIN - Demonstrations
// ============================================

int main(void) {
    printf("NASA RULE 1: RESTRICT CONTROL FLOW\n");
    printf("===================================\n\n");
    
    // Test 1: Data processing
    printf("Test 1: Data Processing\n");
    int data[] = {1, 2, 3, 4, 5};
    int result = good_process_data(data, 5);
    printf("Sum of data: %d\n\n", result);
    
    // Test 2: State machine
    printf("Test 2: State Machine\n");
    printf("IDLE state output: %d\n", good_state_machine(STATE_IDLE, 10));
    printf("RUNNING state output: %d\n", good_state_machine(STATE_RUNNING, 10));
    printf("\n");
    
    // Test 3: Factorial
    printf("Test 3: Iterative Factorial\n");
    printf("5! = %d\n", good_factorial_iterative(5));
    printf("10! = %d\n", good_factorial_iterative(10));
    printf("\n");
    
    // Test 4: Command processing
    printf("Test 4: Command Processing\n");
    Command cmd = parse_command("START");
    execute_command(cmd);
    cmd = parse_command("STATUS");
    execute_command(cmd);
    printf("\n");
    
    // Test 5: Packet processing
    printf("Test 5: Packet Processing\n");
    Packet packet = {
        .header = 0xAA,
        .type = 0x01,
        .length = 5,
        .data = {1, 2, 3, 4, 5},
        .checksum = 1 ^ 2 ^ 3 ^ 4 ^ 5
    };
    process_packet(&packet);
    
    printf("\n✅ Rule 1 Examples Complete\n");
    return 0;
}

/*
 * KEY TAKEAWAYS - RULE 1
 * ======================
 *
 * ✅ DO:
 * - Use early returns for error handling
 * - Use switch/case for state machines
 * - Use iteration instead of recursion
 * - Keep control flow linear and predictable
 * - Use enums for states/commands
 *
 * ❌ DON'T:
 * - Use goto (except for cleanup pattern in specific cases)
 * - Use setjmp/longjmp
 * - Use recursion (direct or indirect)
 * - Create complex branching logic
 * - Jump backwards in code
 *
 * WHY THIS MATTERS:
 * - Predictable execution paths
 * - Easier static analysis
 * - No stack overflow from recursion
 * - Easier to verify correctness
 * - Better for real-time systems
 */
