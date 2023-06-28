CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
CXXFLAGS=-Wall -Wextra -pedantic -O2
TARGET=morse

.PHONY: all default clean

all default: ${TARGET}

${TARGET}.o: ${TARGET}.c ${TARGET}.h makefile

${TARGET}: ${TARGET}.o

clean:
	git clean -dffx
