CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -g

# Individual rule examples
RULE_SOURCES = rule01_control_flow.c \
               rule02_loop_bounds.c \
               rule03_no_dynamic_memory.c

RULE_TARGETS = $(RULE_SOURCES:.c=)

# Main comprehensive example
MAIN_TARGET = nasa_rules

# All targets
ALL_TARGETS = $(MAIN_TARGET) $(RULE_TARGETS)

.PHONY: all clean run test help

all: $(ALL_TARGETS)

# Build individual rule examples
rule01_control_flow: rule01_control_flow.c
	$(CC) $(CFLAGS) -o $@ $<

rule02_loop_bounds: rule02_loop_bounds.c
	$(CC) $(CFLAGS) -o $@ $<

rule03_no_dynamic_memory: rule03_no_dynamic_memory.c
	$(CC) $(CFLAGS) -o $@ $<

# Build main comprehensive example
$(MAIN_TARGET): nasa_rules.c
	$(CC) $(CFLAGS) -o $@ $<

# Run all examples
run: all
	@echo "=== Running Rule 1: Control Flow ==="
	./rule01_control_flow
	@echo ""
	@echo "=== Running Rule 2: Loop Bounds ==="
	./rule02_loop_bounds
	@echo ""
	@echo "=== Running Rule 3: No Dynamic Memory ==="
	./rule03_no_dynamic_memory
	@echo ""
	@echo "=== Running Comprehensive Example ==="
	./$(MAIN_TARGET)

# Run individual examples
run-rule1: rule01_control_flow
	./rule01_control_flow

run-rule2: rule02_loop_bounds
	./rule02_loop_bounds

run-rule3: rule03_no_dynamic_memory
	./rule03_no_dynamic_memory

run-main: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# Static analysis with clang
analyze: $(RULE_SOURCES) nasa_rules.c
	@echo "Running static analysis..."
	clang --analyze $(CFLAGS) $(RULE_SOURCES) nasa_rules.c

# Check with cppcheck (if available)
check:
	@echo "Running cppcheck..."
	@command -v cppcheck >/dev/null 2>&1 && \
		cppcheck --enable=all --suppress=missingIncludeSystem $(RULE_SOURCES) nasa_rules.c || \
		echo "cppcheck not installed, skipping"

# Test compilation with maximum warnings
strict: CFLAGS += -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Wwrite-strings
strict: clean all

# Clean all built files
clean:
	rm -f $(ALL_TARGETS)
	rm -f ex01 ex02 ex03 ex04 ex05 ex06 ex07 ex08 ex09 ex10
	rm -f *.o
	rm -f *.plist
	rm -f *~

# Help
help:
	@echo "NASA Power of 10 Rules - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all examples"
	@echo "  run          - Run all examples"
	@echo "  run-rule1    - Run Rule 1 example only"
	@echo "  run-rule2    - Run Rule 2 example only"
	@echo "  run-rule3    - Run Rule 3 example only"
	@echo "  run-main     - Run comprehensive example"
	@echo "  exercises    - Build all 10 exercises"
	@echo "  ex01-ex10    - Build individual exercises"
	@echo "  analyze      - Run static analysis (clang)"
	@echo "  check        - Run cppcheck"
	@echo "  strict       - Build with maximum warnings"
	@echo "  clean        - Remove all built files"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make                  # Build everything"
	@echo "  make run-rule1        # Run only Rule 1 example"
	@echo "  make analyze          # Static analysis"
	@echo "  make strict           # Extra strict compilation"
