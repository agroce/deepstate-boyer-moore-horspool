CC ?= cc
CFLAGS ?= -O2 -std=c99 -Wall

random_tester: random_tester.c tbm_variants.h
	$(CC) $(CFLAGS) -o $@ random_tester.c

test: random_tester
	./random_tester 2000000

# Quick bounded CBMC checks (minutes); larger bounds: ./run_cbmc.sh VARIANT M N
cbmc-quick:
	./run_cbmc.sh 1 5 10
	./run_cbmc.sh 2 5 10

clean:
	rm -f random_tester cbmc_*.log

.PHONY: test cbmc-quick clean
