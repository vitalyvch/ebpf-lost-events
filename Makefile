all: test_basic
.PHONY: all

CFLAGS += -O2 -Wextra

test_basic: test_basic.c
