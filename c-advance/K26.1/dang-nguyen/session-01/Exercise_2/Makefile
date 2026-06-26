CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := mac_parser
SRC := main.c
INC := -I.

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)