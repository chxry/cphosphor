SRC = src/*.c lib/glad/glad.c lib/log/src/log.c
TARGET = out

CC = clang
CFLAGS = -Wall -O2 -ldl -lSDL2 -Ilib -Ilib/log/src -DLOG_USE_COLOR

$(TARGET): $(SRC)
	${CC} $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

format:
	clang-format -i $(shell find src/ -type f -name '*.[ch]')