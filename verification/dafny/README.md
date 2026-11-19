# Dafny proofs: Horspool and Turbo Boyer-Moore

Full (unbounded) proofs, checked with Dafny 4.9.1, that the search loops return
exactly the set of occurrences of the pattern in the text, for every pattern
of length at least 1, every text, and every character value in `[0, 256)`.

| File | What is proved | Verifies |
|---|---|---|
| `Horspool.dfy` (module `Horspool`) | Boyer-Moore-Horspool: bad-character table built as in the handbook, and the search | 8 conditions, ~8 s |
| `TurboBM.dfy` (module `TurboBMNoFloor`) | Turbo-BM search with the `u + 1` floor removed (conservative fix) | 21 conditions, ~4 s |
| `TurboBM_lecroq.dfy` (module `TurboBMLecroq`) | Turbo-BM search with Lecroq's November 2026 guarded floor | 24 conditions, ~4 s |
| `RunTests.dfy` | Runs the verified Turbo-BM on the known counterexamples | (executable) |

```
dafny verify Horspool.dfy TurboBM.dfy TurboBM_lecroq.dfy    # 53 verified, 0 errors
dafny run --no-verify --target:py RunTests.dfy     # or the default C# target if dotnet is installed
```

Each search method has the postcondition `res == Occurrences(x, y)`, where
`Occurrences` is the set of positions `j` with `x == y[j .. j+|x|]`.

## Scope

- **Search loops: as in the handbook / SMART code.** Right-to-left comparison,
  the jump over the memorized factor (`i -= u`), the turbo shift, the
  bad-character and good-suffix shifts, the memory update, and (in
  `TurboBM_lecroq.dfy`) the guarded floor, written the way the C code does them.
- **Tables: specified by their definitions.** The bad-character table is built
  exactly as `preBmBc` does and proved against its definition. The good-suffix
  table is specified as the *strong* good-suffix shift (least `s >= 1`
  consistent with the matched suffix and with `x[i-s] != x[i]` when
  `i - s >= 0`) and built by a simple verified search. The handbook's
  linear-time `suffixes`/`preBmGs` is **not** verified here; it agrees with the
  specification on all 9.26M table entries for patterns up to length 12 over
  2 and 3 letter alphabets (checked exhaustively in C), and the CBMC harness in
  `..` (the parent `verification` directory) covers it within its bounds.
- Results are sets of positions rather than the C code's `OUTPUT(j)` calls or
  counts.

## The arguments

Every skipped position needs a reason. The lemmas are:

- `GsRuleSafe`, `FullMatchSafe`: the good-suffix shift after a mismatch or a
  full match skips nothing, by minimality of the strong good-suffix shift.
- `BcRuleSafe`: the bad-character shift skips nothing, by the last-occurrence
  property of `bmBc`.
- `TurboRuleSafe`: the turbo shift `u - v` skips nothing. It uses the loop
  invariant that the memorized region (`Mem`) matches the text and is
  `sp`-periodic in the pattern (`Period`), where `sp` is the previous shift.
- `FloorSafe` (Lecroq variant only): when `v < u`, the bad-character shift
  beat the turbo shift, and the memorized factor is not a prefix of the pattern
  (`u < m - sp`), no shift `s` with `u - v < s <= u` is possible. The proof
  needs one more fact carried from the previous attempt, `StrongBefore`: the
  pattern character just before the memorized region differs from
  `x[m-1-u]` (the strong good-suffix inequality at the previous mismatch).
  With `r = m - sp - u`, an occurrence at `j + s` would force

  ```
  x[r-1] = x[r-1+s]       (text position r-1+s is in the memorized region)
         = x[m-1-u+s]     (periodicity)
         = x[m-1-u]       (m-1-u+s is in the matched suffix)
  ```

  contradicting `StrongBefore`. When the memorized factor is a prefix of the
  pattern (`r = 0`), position `r - 1` does not exist and the argument fails;
  that is exactly the original bug.

**Negative control.** Replacing Lecroq's condition
`turboShift < bcShift && v < u && flag` by the original
`turboShift < bcShift` in `TurboBM_lecroq.dfy` makes verification fail at the
`FloorSafe` call, on the `u < m - sp` and `v < u` preconditions.
