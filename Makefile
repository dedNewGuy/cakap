CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

TARGET = cakap
SRCS = main.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
