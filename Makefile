CC = gcc
CFLAGS = -ggdb -Wall -Wextra -I./include

SRCDIR = src
TARGET = cakap
SRCS = $(SRCDIR)/main.c $(SRCDIR)/net.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lpthread

.PHONY: clean run
run:
	./$(TARGET)

clean:
	rm -rf $(TARGET)
