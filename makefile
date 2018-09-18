BIN = statx

all: ${BIN}

${BIN}: statx.c
	gcc -o $@ $^

.PHONY: clean
clean:
	@rm -fv ${BIN}
