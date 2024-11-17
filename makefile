CFLAGS = -I include/ -O2
TEST_FLAGS = -I include/ -g
MEM_CHECK_FLAG = -include include/debug/mem_check.h -Wno-implicit-function-declaration

TEST_C = src/**/*.c
MAIN_C = src/*.c src/**/*.c

TEST_TARGET = tests/programs/bigint.out tests/programs/number.out

all: lreng test

test: ${TEST_TARGET}

debug: CFLAGS = ${TEST_FLAGS}
debug: all

mem_check: CFLAGS = ${TEST_FLAGS} ${MEM_CHECK_FLAG}
mem_check: all

tests/programs/bigint.out: ${TEST_C} tests/programs/bigint.c
	gcc ${TEST_FLAGS} -o $@ ${TEST_C} tests/programs/bigint.c

tests/programs/number.out: ${TEST_C} tests/programs/number.c
	gcc ${TEST_FLAGS} -o $@ ${TEST_C} tests/programs/number.c

lreng: ${MAIN_C}
	gcc ${CFLAGS} -o $@ $^

clean:
	rm lreng ${TEST_TARGET}
