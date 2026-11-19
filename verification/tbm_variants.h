/*
 * Turbo Boyer-Moore (Crochemore et al., Algorithmica 12(4/5), 1994), as given in
 * the C code of Charras and Lecroq's "Exact String Matching Algorithms"
 * (http://www-igm.univ-mlv.fr/~lecroq/string/node15.html) and in SMART's tbm.c,
 * in three variants selected at run time:
 *
 *   TBM_ORIGINAL  the handbook / SMART code as published until November 2026.
 *                 The rule "if (turboShift < bcShift) shift = MAX(shift, u + 1)"
 *                 is unsound: it can skip an occurrence when the memorized
 *                 factor u is a prefix of the pattern (after a full match, or
 *                 after a good-suffix shift that aligned a border of the pattern).
 *   TBM_LECROQ    Thierry Lecroq's November 2026 fix: the u + 1 floor applies only
 *                 when v < u and the memorized factor is not a prefix of the
 *                 pattern (previous shift != m - u).
 *   TBM_NOFLOOR   the conservative fix: the u + 1 floor is removed entirely
 *                 (u = 0 is still reset).
 *
 * All variants report every occurrence (they count them); this matches the
 * handbook and SMART. Code that stops at the first match (MariaDB's LIKE, the
 * FooBarWidget library tested in this repo) can only hit the border-shift path.
 *
 * SIGMA and XSIZE can be overridden before including this file.
 */
#ifndef TBM_VARIANTS_H
#define TBM_VARIANTS_H

#ifndef SIGMA
#define SIGMA 256
#endif
#ifndef XSIZE
#define XSIZE 64
#endif

#define TBM_MAX(a, b) ((a) > (b) ? (a) : (b))
#define TBM_MIN(a, b) ((a) < (b) ? (a) : (b))

enum { TBM_ORIGINAL = 0, TBM_LECROQ = 1, TBM_NOFLOOR = 2 };

static void preBmBc(const unsigned char *x, int m, int bmBc[]) {
   int i;
   for (i = 0; i < SIGMA; ++i)
      bmBc[i] = m;
   for (i = 0; i < m - 1; ++i)
      bmBc[x[i]] = m - i - 1;
}

static void suffixes(const unsigned char *x, int m, int *suff) {
   int f = 0, g, i;   /* f is never read before it is set; initialized for tools */
   suff[m - 1] = m;
   g = m - 1;
   for (i = m - 2; i >= 0; --i) {
      if (i > g && suff[i + m - 1 - f] < i - g)
         suff[i] = suff[i + m - 1 - f];
      else {
         if (i < g)
            g = i;
         f = i;
         while (g >= 0 && x[g] == x[g + m - 1 - f])
            --g;
         suff[i] = f - g;
      }
   }
}

static void preBmGs(const unsigned char *x, int m, int bmGs[]) {
   int i, j, suff[XSIZE];
   suffixes(x, m, suff);
   for (i = 0; i < m; ++i)
      bmGs[i] = m;
   j = 0;
   for (i = m - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < m - 1 - i; ++j)
            if (bmGs[j] == m)
               bmGs[j] = m - 1 - i;
   for (i = 0; i <= m - 2; ++i)
      bmGs[m - 1 - suff[i]] = m - 1 - i;
}

/* Returns the number of occurrences of x (length m >= 1) in y (length n). */
static int tbm_count(int variant, const unsigned char *x, int m,
                     const unsigned char *y, int n) {
   int bcShift, i, j, shift, u, v, turboShift, count = 0;
   int bmGs[XSIZE], bmBc[SIGMA];
   int flag;

   preBmGs(x, m, bmGs);
   preBmBc(x, m, bmBc);

   j = u = 0;
   shift = m;
   while (j <= n - m) {
      i = m - 1;
      while (i >= 0 && x[i] == y[i + j]) {
         --i;
         if (u != 0 && i == m - 1 - shift)
            i -= u;                       /* jump over the memorized factor */
      }
      if (i < 0) {
         count++;                         /* OUTPUT(j) */
         shift = bmGs[0];
         u = m - shift;
      } else {
         v = m - 1 - i;
         turboShift = u - v;
         flag = (shift != m - u);         /* previous shift; u not a prefix of x */
         bcShift = bmBc[y[i + j]] - m + 1 + i;
         shift = TBM_MAX(turboShift, bcShift);
         shift = TBM_MAX(shift, bmGs[i]);
         if (shift == bmGs[i])
            u = TBM_MIN(m - shift, v);
         else {
            if (variant == TBM_ORIGINAL) {
               if (turboShift < bcShift)
                  shift = TBM_MAX(shift, u + 1);          /* the unsound floor */
            } else if (variant == TBM_LECROQ) {
               if (turboShift < bcShift && v < u && flag)
                  shift = TBM_MAX(shift, u + 1);
            }                                             /* TBM_NOFLOOR: none */
            u = 0;
         }
      }
      j += shift;
   }
   return count;
}

/* Reference: naive count of all occurrences. */
static int naive_count(const unsigned char *x, int m, const unsigned char *y, int n) {
   int c = 0, j, k;
   for (j = 0; j + m <= n; j++) {
      for (k = 0; k < m && x[k] == y[j + k]; k++)
         ;
      if (k == m)
         c++;
   }
   return c;
}

#endif
