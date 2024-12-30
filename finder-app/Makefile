CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Wextra -O2
LDFLAGS :=

TARGET := writer
SRC := writer.c
OBJ := writer.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
