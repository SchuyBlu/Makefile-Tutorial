COMPILER=gcc -c
LINKER=gcc
CLFAGS=-Wall -pedantic -g
.PHONY: clean

OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))

main: $(OBJECTS)
	$(LINKER) $^ -o $@

%.o:: %.c %.h
	$(COMPILER) $(CLFAGS) $< -o $@

clean:
	rm -rf *.o
	rm main
