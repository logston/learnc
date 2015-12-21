SHELL = bash

.PHONY: all clean test

all: agent

input.dat: cities.py cities.dat
	./cities.py cities.dat | shuf | head -n 50 > input.dat

agent: agent.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: cities.dat agent agent.py
	for _ in $$(seq 1 5); do \
		departure=$$(shuf cities.dat | head -n1); \
		arrival=$$(shuf cities.dat | head -n1);   \
		diff -s <(./agent.py input.dat route "$$departure" "$$arrival")  \
		        <(./agent    input.dat route "$$departure" "$$arrival"); \
	done
	for _ in $$(seq 1 5); do \
		departure=$$(shuf cities.dat | head -n1); \
		arrival=$$(shuf cities.dat | head -n1);   \
		diff -s <(./agent.py input.dat fare "$$departure" "$$arrival")  \
		        <(./agent    input.dat fare "$$departure" "$$arrival"); \
	done

clean:
	rm -f input.dat test.dat agent
