CC = gcc
CFLAGS = -ggdb -Wall -Wextra

TARGET = cakap
SRCS = main.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

.PHONY: clean run
run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
