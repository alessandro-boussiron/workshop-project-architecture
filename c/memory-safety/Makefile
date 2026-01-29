CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=c11
SANITIZE = -fsanitize=address -fsanitize=undefined
TARGET = memory_safety

all: $(TARGET)

$(TARGET): memory_safety.c
	$(CC) $(CFLAGS) $(SANITIZE) -o $(TARGET) memory_safety.c

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run valgrind
