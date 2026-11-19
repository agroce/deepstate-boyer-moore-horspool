# Checking Turbo Boyer-Moore: targeted random testing and a CBMC harness

Tools for checking the handbook/SMART Turbo Boyer-Moore code against a naive
search, in three variants (`tbm_variants.h`, selected at run time):

| Variant | Number | Code |
|---|---|---|
| original | 0 | handbook / SMART code with the unconditional `u + 1` floor |
| Lecroq fix | 1 | floor applied only when `v < u` and the remembered factor is not a prefix of the pattern |
| floor removed | 2 | no `u + 1` floor at all (`u = 0` still reset) |

All variants count every occurrence, as the handbook and SMART do. Code that
stops at the first match can only hit the border-shift path, not the
full-match path, so for it the `dccacdcc` example below is the relevant one.

## Files

| File | What it is |
|---|---|
| `tbm_variants.h` | The three Turbo-BM variants plus the naive reference; shared by both tools |
| `random_tester.c` | Targeted random differential tester |
| `tbm_harness.c` | CBMC harness for bounded model checking |
| `run_cbmc.sh` | Runs CBMC on the harness with per-loop unwinding bounds |
| `Makefile` | `make`, `make test`, `make cbmc-quick`, `make clean` |
| `dafny/` | Dafny proofs of Horspool and of the Turbo-BM search (floor removed, and Lecroq's fix); see `dafny/README.md` |

## Random tester

```
make test                        # 2M cases, about 2 seconds
./random_tester [iterations] [seed]   # defaults: 10000000 1
```

It first runs the known counterexamples, then random cases, and reports for
each variant the number of wrong answers and the smallest failing case. Exit
status is nonzero if variant 1 or 2 ever disagrees with naive search, so it
can be used as a regression check.

```
Known counterexamples (occurrences found / actual):
  dadbddad         naive=2  orig=1  lecroq=2  nofloor=2
  dccacdcc         naive=1  orig=0  lecroq=1  nofloor=1
  ceebecee         naive=1  orig=0  lecroq=1  nofloor=1
  ACAGGTGCTGGA...  naive=1  orig=0  lecroq=1  nofloor=1

2000000 random cases (seed 1), half border-structured, half uniform:
  original (handbook/SMART)  6526 wrong; smallest: pattern="cddbdcdd" text="eeddbcddcddbdcdd" naive=1 tbm=0
  Lecroq fix (2026)          0 wrong
  floor removed              0 wrong
```

The known counterexamples exercise the two ways in: `dadbddad` in
`dadbddaddadbddad` (the pattern twice) goes through the full-match path;
`dccacdcc` and `ceebecee` go through a good-suffix shift that aligns a border
of the pattern; the long DNA case is the MySQL #59973 query.

### Why the generator is focused

Uniform random strings almost never trigger this bug, even over 2 to 4 letter
alphabets: in our runs, 60 million uniform cases gave at most one failure, and code coverage does not really help a fuzzer reach the problem. The
bug needs coordinated structure to become probable:

1. a pattern with a nontrivial border (a prefix equal to a suffix);
2. an attempt that ends with a full match or a good-suffix shift aligning that
   border, so the remembered factor is a prefix of the pattern;
3. a next attempt where the bad-character shift wins but is no larger than
   `u`, with a real occurrence at exactly that shift.

So half of the cases are built to have that shape:

```
pattern = U + M + U                    U: random border, 1..5 chars; M: 0..7 chars
text    = R + U + (pattern x 0..2) + R'   R: 0..m random chars; R': 0..4
```

over 2 to 5 letter alphabets. The other half are uniform random cases (pattern
length 1..12, text length 0..39, with the pattern planted half the time), so
the tester is not only probing the failure mode we already know about. With
this mix the original code fails thousands of times per million cases.

## CBMC harness

```
./run_cbmc.sh VARIANT M N [TIMEOUT_SECONDS]
make cbmc-quick                  # variants 1 and 2 at M=5, N=10, a few minutes each
```

The harness checks `tbm_count == naive_count`, plus array bounds and signed
overflow, for every pattern of length 1..M and every text of length 0..N.
`run_cbmc.sh` sets an unwinding bound for each loop (every loop is bounded by
M, N or M + 1) and turns on `--unwinding-assertions`, so a successful result
covers all executions within the bound. The full CBMC output, including any
counterexample trace, goes to `cbmc_v<VARIANT>_m<M>_n<N>.log`.

Characters range over M + 1 symbols rather than 256. This loses no
generality for byte strings: text characters that do not occur in the pattern
fail every comparison and all get the same `bmBc` value, so they behave
identically, and pattern characters can be renamed consistently. See the
comment at the top of `tbm_harness.c`. Using all 256 values works but is much
slower.

Results (CBMC 5.95.1, 2-core VM, about 1.5 GB peak memory):

| Variant | Bound | Result | Time |
|---|---|---|---|
| Lecroq fix | M=5, N=10 | VERIFICATION SUCCESSFUL | ~3 min |
| floor removed | M=5, N=10 | VERIFICATION SUCCESSFUL | ~3 min |
| Lecroq fix | M=6, N=12 | VERIFICATION SUCCESSFUL | ~13 min |
| original | M=8, N=16 | VERIFICATION FAILED | ~24 min |

The counterexample CBMC found for the original code has the
`dadbddad`-twice shape. Time grows roughly 4 to 5x per step of (M, N), so
M=8, N=16, the size of the smallest known counterexample, should take a few
hours for the fixed variants.
