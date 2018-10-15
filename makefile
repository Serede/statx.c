CC = gcc
CFLAGS= -Wall -Wno-format -pedantic -ansi
BIN = statx

all: ${BIN}

${BIN}: statx.c
	${CC} ${CFLAGS} -o $@ $^

.PHONY: clean
clean:
	@rm -fv ${BIN}
