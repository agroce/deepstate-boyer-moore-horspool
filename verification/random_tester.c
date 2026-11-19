/*
 * Targeted random differential tester for Turbo Boyer-Moore.
 *
 * Compares each variant in tbm_variants.h against a naive search on random
 * pattern/text pairs and reports the number of wrong answers, plus the
 * smallest failing case seen for each variant.
 *
 * Why "targeted": uniform random strings almost never trigger the bug. In our
 * runs, 60 million uniform cases over 2 to 4 letter alphabets produced at most
 * one failure. The bug needs a pattern with a border (a prefix equal to a
 * suffix) and a text arranged so that a border-aligning shift (or a full
 * match) is followed by a particular bad-character shift. So half of the cases
 * here are built as
 *
 *     pattern = U + M + U            (U a short random border, M random)
 *     text    = R + U + pattern*k + R'   (k = 0..2 copies, R and R' random)
 *
 * and the other half are uniform random. With this generator the original
 * code fails thousands of times per million cases, within seconds.
 *
 * Usage: ./random_tester [iterations] [seed]     (defaults: 10000000, 1)
 * Exit status is nonzero if a fixed variant ever disagrees with naive search.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tbm_variants.h"

static const char *names[3] = { "original (handbook/SMART)", "Lecroq fix (2026)", "floor removed" };

static void show(const unsigned char *s, int len) {
   for (int k = 0; k < len; k++)
      putchar(s[k]);
}

static void check_known(void) {
   /* Known counterexamples for the original code. */
   static const char *cases[][2] = {
      { "dadbddad", "dadbddaddadbddad" },   /* pattern twice: full-match path */
      { "dccacdcc", "abcbadccdccacdcc" },   /* border-aligning good-suffix shift */
      { "ceebecee", "aaaacceeceebecee" },
      /* MySQL Bug #59973 (2011), still present in MariaDB as MDEV-42279 */
      { "ACAGGTGCTGGAGAGGATGCGGAGAAATAGGAACA",
        "CAAAACCACTATGAGATATCATCTCACACCAGTTAGAATGGCAATCATTAAAAAGTCAGG"
        "AAACAACAGGTGCTGGAGAGGATGCGGAGAAATAGGAACAC" },
   };
   printf("Known counterexamples (occurrences found / actual):\n");
   for (unsigned c = 0; c < sizeof cases / sizeof cases[0]; c++) {
      const unsigned char *x = (const unsigned char *)cases[c][0];
      const unsigned char *y = (const unsigned char *)cases[c][1];
      int m = (int)strlen(cases[c][0]), n = (int)strlen(cases[c][1]);
      printf("  %-12.12s%s  naive=%d", cases[c][0], m > 12 ? "..." : "   ", naive_count(x, m, y, n));
      for (int v = 0; v < 3; v++)
         printf("  %s=%d", v == 0 ? "orig" : v == 1 ? "lecroq" : "nofloor", tbm_count(v, x, m, y, n));
      printf("\n");
   }
   printf("\n");
}

int main(int argc, char **argv) {
   long iters = argc > 1 ? atol(argv[1]) : 10000000L;
   unsigned seed = argc > 2 ? (unsigned)atoi(argv[2]) : 1u;
   long bad[3] = { 0, 0, 0 };
   int best[3] = { 1 << 30, 1 << 30, 1 << 30 };
   unsigned char bx[3][64], by[3][128];
   int bm[3], bn[3];

   check_known();
   srand(seed);
   for (long it = 0; it < iters; it++) {
      unsigned char x[64], y[128];
      int A = 2 + rand() % 4, m, n = 0;
      if (it & 1) {                                  /* border-structured case */
         int ul = 1 + rand() % 5, ml = rand() % 8;
         unsigned char U[8];
         for (int k = 0; k < ul; k++) U[k] = 'a' + rand() % A;
         m = 0;
         for (int k = 0; k < ul; k++) x[m++] = U[k];
         for (int k = 0; k < ml; k++) x[m++] = 'a' + rand() % A;
         for (int k = 0; k < ul; k++) x[m++] = U[k];
         int pre = rand() % (m + 1);
         for (int k = 0; k < pre; k++) y[n++] = 'a' + rand() % A;
         for (int k = 0; k < ul; k++) y[n++] = U[k];
         int reps = rand() % 3;
         for (int r = 0; r < reps; r++)
            for (int k = 0; k < m; k++) y[n++] = x[k];
         int post = rand() % 5;
         for (int k = 0; k < post; k++) y[n++] = 'a' + rand() % A;
      } else {                                       /* uniform case */
         m = 1 + rand() % 12;
         for (int k = 0; k < m; k++) x[k] = 'a' + rand() % A;
         n = rand() % 40;
         for (int k = 0; k < n; k++) y[k] = 'a' + rand() % A;
         if (n >= m && rand() % 2) memcpy(y + rand() % (n - m + 1), x, m);
      }
      int want = naive_count(x, m, y, n);
      for (int v = 0; v < 3; v++) {
         if (tbm_count(v, x, m, y, n) != want) {
            bad[v]++;
            if (m + n < best[v]) {
               best[v] = m + n; bm[v] = m; bn[v] = n;
               memcpy(bx[v], x, m); memcpy(by[v], y, n);
            }
         }
      }
   }

   printf("%ld random cases (seed %u), half border-structured, half uniform:\n", iters, seed);
   for (int v = 0; v < 3; v++) {
      printf("  %-26s %ld wrong", names[v], bad[v]);
      if (bad[v]) {
         printf("; smallest: pattern=\""); show(bx[v], bm[v]);
         printf("\" text=\""); show(by[v], bn[v]);
         printf("\" naive=%d tbm=%d", naive_count(bx[v], bm[v], by[v], bn[v]),
                tbm_count(v, bx[v], bm[v], by[v], bn[v]));
      }
      printf("\n");
   }
   return (bad[1] || bad[2]) ? 1 : 0;
}
