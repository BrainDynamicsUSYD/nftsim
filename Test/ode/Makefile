CPP = $(wildcard *.cpp)

default: run

.PHONY: run

run: a.out
	./$< |tee output.txt

a.out: $(CPP)
	g++ $<
