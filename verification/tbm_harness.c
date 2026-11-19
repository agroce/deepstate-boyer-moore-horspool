/*
 * CBMC harness: bounded model checking that Turbo Boyer-Moore counts
 * occurrences correctly, for every pattern of length 1..M and every text of
 * length 0..N.
 *
 * Property: tbm_count(VARIANT, x, m, y, n) == naive_count(x, m, y, n), checked
 * together with array bounds and signed overflow. run_cbmc.sh adds
 * --unwinding-assertions, so a "VERIFICATION SUCCESSFUL" result covers all
 * executions within the bound, not just those that fit in the unrolling.
 *
 * VARIANT (see tbm_variants.h): 0 = original handbook/SMART code (expected to
 * FAIL once M >= 8 and N >= 16), 1 = Lecroq's 2026 fix, 2 = floor removed.
 *
 * Alphabet reduction. Characters are restricted to SIGMA = M + 1 values, and
 * this loses no generality for byte strings:
 *   - Both algorithms only compare characters for equality and look them up in
 *     bmBc. Any text character that does not occur in the pattern fails every
 *     comparison and has bmBc value m, so all such characters behave
 *     identically and can be mapped to a single symbol.
 *   - The pattern has at most M distinct characters, which can be renamed
 *     consistently in pattern and text without changing either result.
 *   So every pair of byte strings with m <= M, n <= N maps to a pair over
 *   M + 1 symbols with the same naive count and the same tbm_count, and a
 *   check over M + 1 symbols covers all 256 byte values. (Using SIGMA = 256
 *   directly is correct but far slower.)
 *
 * Results on a 2-core cloud VM with CBMC 5.95.1 (about 1.5 GB peak memory):
 *   VARIANT=1 M=5 N=10   VERIFICATION SUCCESSFUL   ~3 min
 *   VARIANT=1 M=6 N=12   VERIFICATION SUCCESSFUL   ~13 min
 *   VARIANT=0 M=8 N=16   VERIFICATION FAILED       ~24 min (pattern "dadbddad"
 *                        shape, text = the pattern twice)
 * Time grows roughly 4 to 5x per step of (M, N).
 */
#include <assert.h>

#ifndef M
#define M 8
#endif
#ifndef N
#define N 16
#endif
#ifndef VARIANT
#define VARIANT 1
#endif

#define SIGMA (M + 1)      /* see "Alphabet reduction" above */
#define XSIZE (M)
#include "tbm_variants.h"

unsigned char nondet_uchar(void);
int nondet_int(void);

int main(void) {
   unsigned char x[M], y[N];
   for (int k = 0; k < M; k++) { x[k] = nondet_uchar(); __CPROVER_assume(x[k] < SIGMA); }
   for (int k = 0; k < N; k++) { y[k] = nondet_uchar(); __CPROVER_assume(y[k] < SIGMA); }
   int m = nondet_int(), n = nondet_int();
   __CPROVER_assume(1 <= m && m <= M && 0 <= n && n <= N);
   assert(tbm_count(VARIANT, x, m, y, n) == naive_count(x, m, y, n));
   return 0;
}
