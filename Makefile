CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude -O2 -std=c99 -D_GNU_SOURCE
LDFLAGS =

# Core sources
SRC = src/main.c src/terminal.c src/input.c src/pager.c src/modules.c
OBJ    = $(SRC:.c=.o)
TARGET = some

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Generic rule: every .c depends on some.h
%.o: %.c include/some.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
