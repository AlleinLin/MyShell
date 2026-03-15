TARGET = myshell
SRC = shell.c

CC = gcc

CFLAGS = -Wall -g

$(TARGET): $(SRC)
    $(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY: clean
clean:
    rm -f $(TARGET)