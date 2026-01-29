CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11
TARGET = layered_arch

all: $(TARGET)

$(TARGET): layered_arch.c
	$(CC) $(CFLAGS) -o $(TARGET) layered_arch.c

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
