#!/bin/bash
# Bounded model checking of Turbo Boyer-Moore with CBMC (tbm_harness.c).
# usage: ./run_cbmc.sh VARIANT M N [TIMEOUT_SECONDS]
#   VARIANT 0 = original (expect FAILED at M=8 N=16), 1 = Lecroq fix, 2 = floor removed
# Writes the full CBMC output (including any counterexample trace) to
# cbmc_v<VARIANT>_m<M>_n<N>.log
#
# Unwinding: every loop is bounded by M, N or SIGMA = M + 1 (the search loop
# runs at most n - m + 1 times because every shift is at least 1), so per-loop
# bounds are given; --unwinding-assertions checks that they are sufficient.
set -u
V=${1:?VARIANT}; M=${2:?M}; N=${3:?N}; T=${4:-86400}
U="main.0:$((M+2)),main.1:$((N+2))"
U+=",preBmBc.0:$((M+3)),preBmBc.1:$((M+2))"
U+=",suffixes.0:$((M+2)),suffixes.1:$((M+2))"
U+=",preBmGs.0:$((M+2)),preBmGs.1:$((M+2)),preBmGs.2:$((M+2)),preBmGs.3:$((M+2))"
U+=",tbm_count.0:$((M+2)),tbm_count.1:$((N+3))"
U+=",naive_count.0:$((M+2)),naive_count.1:$((N+3))"
LOG=cbmc_v${V}_m${M}_n${N}.log
start=$(date +%s)
timeout "$T" cbmc tbm_harness.c -DVARIANT="$V" -DM="$M" -DN="$N" \
  --unwindset "$U" --unwinding-assertions --bounds-check --signed-overflow-check \
  --trace > "$LOG" 2>&1
rc=$?
echo "VARIANT=$V M=$M N=$N exit=$rc time=$(( $(date +%s) - start ))s $(grep -E '^VERIFICATION' "$LOG")"
[ $rc -eq 124 ] && echo "(timed out)"
exit $rc
