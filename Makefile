CC = gcc
CFLAGS = -Wall
TARGET = bfuck

all: $(TARGET)

$(TARGET): brain_fuck.c
	$(CC) $(CFLAGS) -o $(TARGET) $^

debug: brain_fuck.c
	$(CC) $(CFLAGS) -o $(TARGET) $^ -g
