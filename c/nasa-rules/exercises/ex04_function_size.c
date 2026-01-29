/*
 * EXERCISE 4: FUNCTION SIZE LIMIT
 * 
 * Task: Refactor large functions into smaller ones
 * - Each function < 60 lines
 * - Single responsibility per function
 * - Clear function names
 * 
 * Compile: gcc -Wall -Wextra -Werror -std=c11 ex04_function_size.c -o ex04
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MAX_PACKETS 10
#define MAX_PACKET_SIZE 64

typedef struct {
    uint16_t id;
    uint8_t data[MAX_PACKET_SIZE];
    size_t size;
    uint32_t checksum;
    bool valid;
} Packet;

// ============================================
// ❌ BAD CODE TO FIX - TOO LARGE
// ============================================

/* Problem: 100+ line function doing everything */
int bad_process_packets(Packet *packets, size_t count) {
    if (packets == NULL || count == 0) {
        return -1;
    }
    
    // Validate all packets
    for (size_t i = 0; i < count; i++) {
        if (packets[i].size > MAX_PACKET_SIZE) {
            packets[i].valid = false;
            continue;
        }
        
        // Calculate checksum
        uint32_t checksum = 0;
        for (size_t j = 0; j < packets[i].size; j++) {
            checksum += packets[i].data[j];
        }
        checksum = checksum ^ 0xFFFFFFFF;
        
        // Verify checksum
        if (checksum != packets[i].checksum) {
            packets[i].valid = false;
            continue;
        }
        
        packets[i].valid = true;
    }
    
    // Sort packets by ID
    for (size_t i = 0; i < count - 1; i++) {
        for (size_t j = 0; j < count - i - 1; j++) {
            if (packets[j].id > packets[j + 1].id) {
                Packet temp = packets[j];
                packets[j] = packets[j + 1];
                packets[j + 1] = temp;
            }
        }
    }
    
    // Find duplicates
    int duplicates = 0;
    for (size_t i = 0; i < count - 1; i++) {
        if (packets[i].id == packets[i + 1].id) {
            duplicates++;
            packets[i + 1].valid = false;
        }
    }
    
    // Calculate statistics
    int valid_count = 0;
    size_t total_bytes = 0;
    uint16_t min_id = 0xFFFF;
    uint16_t max_id = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (packets[i].valid) {
            valid_count++;
            total_bytes += packets[i].size;
            
            if (packets[i].id < min_id) {
                min_id = packets[i].id;
            }
            if (packets[i].id > max_id) {
                max_id = packets[i].id;
            }
        }
    }
    
    // Print report
    printf("=== Packet Processing Report ===\n");
    printf("Total packets: %zu\n", count);
    printf("Valid packets: %d\n", valid_count);
    printf("Invalid packets: %zu\n", count - valid_count);
    printf("Duplicates found: %d\n", duplicates);
    printf("Total bytes: %zu\n", total_bytes);
    printf("ID range: %u - %u\n", min_id, max_id);
    
    return valid_count;
}

// ============================================
// ✅ YOUR TASK: REFACTOR INTO SMALL FUNCTIONS
// ============================================

/* TODO: Function 1 - Calculate checksum
 * Max 10 lines
 */
uint32_t calculate_checksum(const uint8_t *data, size_t size) {
    // TODO: Implement checksum calculation
    (void)data;
    (void)size;
    return 0;
}

/* TODO: Function 2 - Validate single packet
 * Max 15 lines
 */
bool validate_packet(Packet *packet) {
    // TODO: Check size and verify checksum
    (void)packet;
    return false;
}

/* TODO: Function 3 - Validate all packets
 * Max 10 lines
 */
int validate_all_packets(Packet *packets, size_t count) {
    // TODO: Iterate and validate each
    (void)packets;
    (void)count;
    return 0;
}

/* TODO: Function 4 - Swap two packets
 * Max 5 lines
 */
void swap_packets(Packet *a, Packet *b) {
    // TODO: Implement swap
    (void)a;
    (void)b;
}

/* TODO: Function 5 - Sort packets by ID
 * Max 15 lines
 */
void sort_packets_by_id(Packet *packets, size_t count) {
    // TODO: Bubble sort using swap_packets
    (void)packets;
    (void)count;
}

/* TODO: Function 6 - Mark duplicates invalid
 * Max 15 lines
 * Returns: number of duplicates found
 */
int mark_duplicate_packets(Packet *packets, size_t count) {
    // TODO: Find and mark duplicates
    (void)packets;
    (void)count;
    return 0;
}

/* TODO: Function 7 - Calculate packet statistics
 * Max 20 lines
 */
typedef struct {
    int valid_count;
    size_t total_bytes;
    uint16_t min_id;
    uint16_t max_id;
} PacketStats;

PacketStats calculate_packet_stats(const Packet *packets, size_t count) {
    PacketStats stats = {0};
    // TODO: Calculate all statistics
    (void)packets;
    (void)count;
    return stats;
}

/* TODO: Function 8 - Print report
 * Max 15 lines
 */
void print_packet_report(size_t total, int duplicates, 
                         const PacketStats *stats) {
    // TODO: Print formatted report
    (void)total;
    (void)duplicates;
    (void)stats;
}

/* TODO: Function 9 - Main orchestrator
 * Max 20 lines
 * Should call all above functions
 */
int good_process_packets(Packet *packets, size_t count) {
    // TODO: Orchestrate packet processing
    // 1. Validate all
    // 2. Sort
    // 3. Mark duplicates
    // 4. Calculate stats
    // 5. Print report
    (void)packets;
    (void)count;
    return 0;
}

// ============================================
// TEST HARNESS
// ============================================

void setup_test_packets(Packet *packets, size_t count) {
    // Packet 0: Valid
    packets[0].id = 100;
    packets[0].size = 4;
    packets[0].data[0] = 0x01;
    packets[0].data[1] = 0x02;
    packets[0].data[2] = 0x03;
    packets[0].data[3] = 0x04;
    packets[0].checksum = calculate_checksum(packets[0].data, packets[0].size);
    
    // Packet 1: Valid
    packets[1].id = 50;
    packets[1].size = 3;
    packets[1].data[0] = 0xAA;
    packets[1].data[1] = 0xBB;
    packets[1].data[2] = 0xCC;
    packets[1].checksum = calculate_checksum(packets[1].data, packets[1].size);
    
    // Packet 2: Duplicate of packet 1
    packets[2].id = 50;
    packets[2].size = 3;
    packets[2].data[0] = 0xAA;
    packets[2].data[1] = 0xBB;
    packets[2].data[2] = 0xCC;
    packets[2].checksum = calculate_checksum(packets[2].data, packets[2].size);
    
    // Packet 3: Invalid checksum
    packets[3].id = 200;
    packets[3].size = 2;
    packets[3].data[0] = 0xFF;
    packets[3].data[1] = 0xFF;
    packets[3].checksum = 0xDEADBEEF;  // Wrong checksum
}

void test_small_functions(void) {
    printf("Test 1: Individual Functions\n");
    
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint32_t checksum = calculate_checksum(data, 3);
    printf("  Checksum: 0x%08X\n", checksum);
    
    Packet test_packet = {0};
    test_packet.size = 3;
    memcpy(test_packet.data, data, 3);
    test_packet.checksum = checksum;
    
    bool valid = validate_packet(&test_packet);
    printf("  Packet valid: %s\n\n", valid ? "true" : "false");
}

void test_bad_version(void) {
    printf("Test 2: Bad Version (Large Function)\n");
    
    Packet packets[4] = {0};
    setup_test_packets(packets, 4);
    
    int valid = bad_process_packets(packets, 4);
    printf("Valid packets: %d\n\n", valid);
}

void test_good_version(void) {
    printf("Test 3: Good Version (Refactored)\n");
    
    Packet packets[4] = {0};
    setup_test_packets(packets, 4);
    
    int valid = good_process_packets(packets, 4);
    printf("Valid packets: %d\n\n", valid);
}

int main(void) {
    printf("EXERCISE 4: FUNCTION SIZE LIMIT\n");
    printf("================================\n\n");
    
    test_small_functions();
    test_bad_version();
    test_good_version();
    
    printf("✅ Exercise 4 complete!\n");
    printf("\nHints:\n");
    printf("1. Each function should do ONE thing\n");
    printf("2. Extract loops into separate functions\n");
    printf("3. Use helper functions for calculations\n");
    printf("4. Create struct for grouped data (PacketStats)\n");
    printf("5. Main function should be high-level orchestration\n");
    printf("\nCount your lines - no function should exceed 60!\n");
    
    return 0;
}

/*
 * GRADING CRITERIA
 * ================
 * 
 * [ ] All functions < 60 lines
 * [ ] Each function has single responsibility
 * [ ] Clear, descriptive function names
 * [ ] Proper parameter passing
 * [ ] No duplicate code
 * [ ] Code compiles without warnings
 * [ ] All tests pass
 * 
 * BONUS POINTS
 * ============
 * 
 * [ ] Most functions < 20 lines
 * [ ] Add function documentation
 * [ ] Zero code duplication
 * [ ] Extract magic numbers to constants
 */
