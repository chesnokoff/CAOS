.PHONY: all build run clean

all: build

clean:
	${RM} calculation.c
	${RM} calculation

build: 1.c
	gcc -g -o main 1.c

run:
	./main

debug: main
	gdb ./main
