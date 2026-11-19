// Turbo Boyer-Moore (handbook/SMART search loop), verified in Dafny.
// Variant proved here: the u + 1 floor removed (the conservative fix).
//
// The tables are specified by their definitions (strong good-suffix shift,
// last-occurrence bad-character shift) and computed by simple verified
// builders; the linear-time preBmGs of the handbook is not verified here
// (it agrees with StrongGs on all 9.26M entries for patterns up to length 12
// over 2 and 3 letter alphabets, checked exhaustively in C).

const SIGMA: int := 256

predicate ValidChars(s: seq<int>) { forall k :: 0 <= k < |s| ==> 0 <= s[k] < SIGMA }

predicate MatchAt(x: seq<int>, y: seq<int>, j: int)
{
  0 <= j && j + |x| <= |y| && forall k :: 0 <= k < |x| ==> x[k] == y[j + k]
}

function Occurrences(x: seq<int>, y: seq<int>): set<int>
{
  set j | 0 <= j <= |y| - |x| && MatchAt(x, y, j)
}

// ---------------------------------------------------------------- tables

predicate BcSafe(x: seq<int>, bc: seq<int>)
  requires |x| >= 1
{
  |bc| == SIGMA &&
  forall c :: 0 <= c < SIGMA ==>
    1 <= bc[c] <= |x| &&
    forall k :: |x| - 1 - bc[c] < k <= |x| - 2 ==> x[k] != c
}

method PreBmBc(x: seq<int>) returns (bc: array<int>)
  requires |x| >= 1 && ValidChars(x)
  ensures BcSafe(x, bc[..])
{
  var m := |x|;
  bc := new int[SIGMA];
  var i := 0;
  while i < SIGMA
    invariant 0 <= i <= SIGMA
    invariant forall c :: 0 <= c < i ==> bc[c] == m
  {
    bc[i] := m;
    i := i + 1;
  }
  i := 0;
  while i < m - 1
    invariant 0 <= i <= if m - 1 < 0 then 0 else m - 1
    invariant forall c :: 0 <= c < SIGMA ==>
      1 <= bc[c] <= m &&
      (bc[c] == m || (m - 1 - bc[c] < i && x[m - 1 - bc[c]] == c)) &&
      forall k :: m - 1 - bc[c] < k < i ==> x[k] != c
  {
    bc[x[i]] := m - i - 1;
    i := i + 1;
  }
}

// Shift s is consistent with having matched x[i+1..m-1] and mismatched at i.
predicate Consistent(x: seq<int>, i: int, s: int)
{
  forall k :: i < k < |x| && 0 <= k && 0 <= k - s < |x| ==> x[k - s] == x[k]
}

predicate GsOK(x: seq<int>, i: int, s: int)
  requires 0 <= i < |x|
{
  1 <= s && Consistent(x, i, s) && (i - s >= 0 ==> x[i - s] != x[i])
}

// gs[i] is the strong good-suffix shift: the least s >= 1 satisfying GsOK.
predicate GsStrong(x: seq<int>, gs: seq<int>)
{
  |gs| == |x| &&
  forall i :: 0 <= i < |x| ==>
    1 <= gs[i] <= |x| && GsOK(x, i, gs[i]) &&
    forall s :: 1 <= s < gs[i] ==> !GsOK(x, i, s)
}

method StrongGs(x: seq<int>) returns (gs: array<int>)
  requires |x| >= 1
  ensures GsStrong(x, gs[..])
{
  var m := |x|;
  gs := new int[m];
  var i := 0;
  while i < m
    invariant 0 <= i <= m
    invariant forall i' :: 0 <= i' < i ==>
      1 <= gs[i'] <= m && GsOK(x, i', gs[i']) &&
      forall s :: 1 <= s < gs[i'] ==> !GsOK(x, i', s)
  {
    assert GsOK(x, i, m);
    var s := 1;
    while !GsOK(x, i, s)
      invariant 1 <= s <= m
      invariant forall t :: 1 <= t < s ==> !GsOK(x, i, t)
      decreases m - s
    {
      s := s + 1;
    }
    gs[i] := s;
    i := i + 1;
  }
}

// ---------------------------------------------------------------- shift safety

// After a mismatch at i, no occurrence at j+s for 1 <= s < gs[i].
lemma GsRuleSafe(x: seq<int>, y: seq<int>, gs: seq<int>, j: int, i: int, s: int)
  requires GsStrong(x, gs) && 0 <= i < |x| && 0 <= j && j + |x| <= |y|
  requires y[j + i] != x[i] && forall k :: i < k < |x| ==> y[j + k] == x[k]
  requires 1 <= s < gs[i]
  ensures !MatchAt(x, y, j + s)
{
  if MatchAt(x, y, j + s) {
    forall k | i < k < |x| && k - s >= 0 ensures x[k - s] == x[k] {
      assert x[k - s] == y[(j + s) + (k - s)];
    }
    if i - s >= 0 { assert x[i - s] == y[(j + s) + (i - s)]; }
    assert GsOK(x, i, s);
  }
}

// After a full match at j, no occurrence at j+s for 1 <= s < gs[0].
lemma FullMatchSafe(x: seq<int>, y: seq<int>, gs: seq<int>, j: int, s: int)
  requires |x| >= 1 && GsStrong(x, gs) && MatchAt(x, y, j)
  requires 1 <= s < gs[0]
  ensures !MatchAt(x, y, j + s)
{
  if MatchAt(x, y, j + s) {
    forall k | 0 < k < |x| && k - s >= 0 ensures x[k - s] == x[k] {
      assert x[k - s] == y[(j + s) + (k - s)];
      assert x[k] == y[j + k];
    }
    assert GsOK(x, 0, s);
  }
}

// Bad character: no occurrence at j+s for 1 <= s < bc[c] - (m-1-i), c = y[j+i].
lemma BcRuleSafe(x: seq<int>, y: seq<int>, bc: seq<int>, j: int, i: int, s: int)
  requires |x| >= 1 && BcSafe(x, bc) && ValidChars(y)
  requires 0 <= i < |x| && 0 <= j && j + |x| <= |y|
  requires 1 <= s < bc[y[j + i]] - |x| + 1 + i
  ensures !MatchAt(x, y, j + s)
{
  if MatchAt(x, y, j + s) {
    var c := y[j + i];
    assert x[i - s] == y[(j + s) + (i - s)];
    assert i - s <= |x| - 2 && |x| - 1 - bc[c] < i - s;
  }
}

// The memorized factor: in window j, positions [m-sp-u, m-sp) of the text are
// known to match the pattern, and the pattern is sp-periodic there.
predicate Mem(x: seq<int>, y: seq<int>, j: int, u: int, sp: int)
{
  j + |x| <= |y| ==>
    forall k :: |x| - sp - u <= k < |x| - sp && 0 <= k < |x| && 0 <= j + k < |y| ==> y[j + k] == x[k]
}
predicate Period(x: seq<int>, u: int, sp: int)
{
  forall k :: |x| - sp - u <= k < |x| - sp && 0 <= k < |x| && 0 <= k + sp < |x| ==> x[k] == x[k + sp]
}

// Turbo shift: after matching v < u characters and mismatching at i = m-1-v,
// no occurrence at j+s for 1 <= s < u - v.
lemma TurboRuleSafe(x: seq<int>, y: seq<int>, j: int, i: int, u: int, sp: int, s: int)
  requires 0 <= i < |x| && 0 <= j && j + |x| <= |y|
  requires 1 <= sp <= |x| && 0 <= u <= |x| - sp
  requires Mem(x, y, j, u, sp) && Period(x, u, sp)
  requires y[j + i] != x[i] && forall k :: i < k < |x| ==> y[j + k] == x[k]
  requires 1 <= s < u - (|x| - 1 - i)
  ensures !MatchAt(x, y, j + s)
{
  if MatchAt(x, y, j + s) {
    var m := |x|;
    var k := i - sp;                 // in the memorized region
    assert m - sp - u <= k < m - sp;
    assert y[j + k] == x[k] && x[k] == x[i];
    var k2 := k - s;                 // also in the region
    assert m - sp - u <= k2 < m - sp;
    assert x[k2] == y[(j + s) + k2];  // so x[k2] == x[i]
    assert x[k2] == x[k2 + sp];       // and x[k2 + sp] == x[i - s]
    assert x[i - s] == y[(j + s) + (i - s)];  // == y[j+i] != x[i]
  }
}

// ---------------------------------------------------------------- search

method TurboBM(x: seq<int>, y: seq<int>) returns (res: set<int>)
  requires |x| >= 1 && ValidChars(x) && ValidChars(y)
  ensures res == Occurrences(x, y)
{
  var m := |x|;
  var n := |y|;
  var bc := PreBmBc(x);
  var gs := StrongGs(x);
  var j := 0;
  var u := 0;
  var shift := m;
  res := {};
  while j <= n - m
    invariant 0 <= j && 1 <= shift <= m && 0 <= u <= m - shift
    invariant res == set p | 0 <= p < j && p <= n - m && MatchAt(x, y, p)
    invariant Mem(x, y, j, u, shift) && Period(x, u, shift)
    decreases n - m - j
  {
    var i := m - 1;
    while i >= 0 && x[i] == y[i + j]
      invariant -1 <= i <= m - 1
      invariant forall k :: i < k < m ==> x[k] == y[j + k]
      invariant u > 0 ==> (i >= m - shift || i < m - shift - u)
    {
      i := i - 1;
      if u != 0 && i == m - 1 - shift {
        i := i - u;                  // jump over the memorized factor
      }
    }
    var newShift: int;
    var newU: int;
    ghost var oldRes := res;
    ghost var wasMatch := i < 0;
    if i < 0 {
      assert MatchAt(x, y, j);
      res := res + {j};
      newShift := gs[0];
      newU := m - newShift;
      forall p | j < p < j + newShift ensures !MatchAt(x, y, p) {
        FullMatchSafe(x, y, gs[..], j, p - j);
      }
      // new memorized factor: y[j+newShift .. j+m-1] == x[newShift .. m-1] == x[0 .. m-newShift-1]
      forall k | 0 <= k < m - newShift ensures x[k] == x[k + newShift] {
        assert Consistent(x, 0, newShift);
      }
      forall k | 0 <= k < m - newShift && j + newShift + k < n
        ensures y[j + newShift + k] == x[k]
      {
        assert y[j + (newShift + k)] == x[newShift + k];
      }
      assert Mem(x, y, j + newShift, newU, newShift);
    } else {
      assert !MatchAt(x, y, j);
      var v := m - 1 - i;
      var turboShift := u - v;
      var bcShift := bc[y[i + j]] - m + 1 + i;
      newShift := if turboShift > bcShift then turboShift else bcShift;
      newShift := if newShift > gs[i] then newShift else gs[i];
      forall p | j < p < j + newShift ensures !MatchAt(x, y, p) {
        var s := p - j;
        if s < gs[i] {
          GsRuleSafe(x, y, gs[..], j, i, s);
        } else if s < bcShift {
          BcRuleSafe(x, y, bc[..], j, i, s);
        } else {
          TurboRuleSafe(x, y, j, i, u, shift, s);
        }
      }
      if newShift == gs[i] {
        newU := if m - newShift < v then m - newShift else v;
        forall k | m - newShift - newU <= k < m - newShift ensures x[k] == x[k + newShift] {
          assert Consistent(x, i, newShift);
        }
        forall k | m - newShift - newU <= k < m - newShift && 0 <= k && j + newShift + k < n
          ensures y[j + newShift + k] == x[k]
        {
          assert i < newShift + k < m;
          assert y[j + (newShift + k)] == x[newShift + k];
        }
        assert Mem(x, y, j + newShift, newU, newShift);
      } else {
        // floor removed: no "shift = MAX(shift, u + 1)"
        newU := 0;
      }
    }
    assert wasMatch <==> MatchAt(x, y, j);
    assert res == oldRes + (if MatchAt(x, y, j) then {j} else {});
    assert forall p :: j < p < j + newShift ==> !MatchAt(x, y, p);
    assert res == set p | 0 <= p < j + newShift && p <= n - m && MatchAt(x, y, p) by {
      forall p ensures p in res <==> (0 <= p < j + newShift && p <= n - m && MatchAt(x, y, p)) {
        if j < p < j + newShift { assert !MatchAt(x, y, p); }
      }
    }
    j := j + newShift;
    shift := newShift;
    u := newU;
  }
}
