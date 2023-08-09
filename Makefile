CC=clang
CFLAGS=-g -Wall

BIN=strings

run: build
	./$(BIN)

build: $(BIN)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN) *.dSYM
