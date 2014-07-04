CC = gcc
CFLAGS = -g -Wall
TARGET = brain_fuck_int

all: $(TARGET)

$(TARGET): brain_fuck.c
	$(CC) $(CFLAGS) -o $(TARGET) $^
