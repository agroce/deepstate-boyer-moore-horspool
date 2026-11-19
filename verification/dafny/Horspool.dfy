// Boyer-Moore-Horspool, verified in Dafny.
// Characters are ints in [0, SIGMA); the pattern x is nonempty.

module Horspool {

  const SIGMA: int := 256

  predicate ValidChars(s: seq<int>) { forall k :: 0 <= k < |s| ==> 0 <= s[k] < SIGMA }

  // x occurs in y at position j
  predicate MatchAt(x: seq<int>, y: seq<int>, j: int)
  {
    0 <= j && j + |x| <= |y| && forall k :: 0 <= k < |x| ==> x[k] == y[j + k]
  }

  function Occurrences(x: seq<int>, y: seq<int>): set<int>
  {
    set j | 0 <= j <= |y| - |x| && MatchAt(x, y, j)
  }

  // The bad-character table: bc[c] = m - 1 - (last position of c in x[0..m-2]), or m.
  // Only the "safe" half is needed for correctness: no occurrence of c in x
  // strictly after position m-1-bc[c] (up to m-2), and bc[c] >= 1.
  predicate BcSafe(x: seq<int>, bc: seq<int>)
    requires |x| >= 1
  {
    |bc| == SIGMA &&
    forall c :: 0 <= c < SIGMA ==>
      1 <= bc[c] <= |x| &&
      forall k :: |x| - 1 - bc[c] < k <= |x| - 2 ==> x[k] != c
  }

  // preBmBc, as in the handbook code
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
      // bc[c] reflects the last occurrence of c in x[0..i-1]
      invariant forall c :: 0 <= c < SIGMA ==>
        1 <= bc[c] <= m &&
        (bc[c] == m || (m - 1 - bc[c] < i && x[m - 1 - bc[c]] == c)) &&
        forall k :: m - 1 - bc[c] < k < i ==> x[k] != c
    {
      bc[x[i]] := m - i - 1;
      i := i + 1;
    }
  }

  // The one-line safety argument: after looking at window j, no occurrence
  // starts strictly between j and j + bc[y[j+m-1]].
  lemma HorspoolSkipSafe(x: seq<int>, y: seq<int>, bc: seq<int>, j: int, p: int)
    requires |x| >= 1 && BcSafe(x, bc) && ValidChars(y)
    requires 0 <= j && j + |x| <= |y|
    requires j < p < j + bc[y[j + |x| - 1]]
    ensures !MatchAt(x, y, p)
  {
    if MatchAt(x, y, p) {
      var m := |x|;
      var c := y[j + m - 1];
      var k := j + m - 1 - p;       // pattern index aligned with y[j+m-1] in window p
      assert 0 <= k <= m - 2;
      assert x[k] == y[p + k];      // from MatchAt at p
      assert x[k] == c;
      assert m - 1 - bc[c] < k;     // contradicts BcSafe
    }
  }

  method Horspool(x: seq<int>, y: seq<int>) returns (res: set<int>)
    requires |x| >= 1 && ValidChars(x) && ValidChars(y)
    ensures res == Occurrences(x, y)
  {
    var m := |x|;
    var n := |y|;
    var bc := PreBmBc(x);
    var j := 0;
    res := {};
    while j <= n - m
      invariant 0 <= j
      invariant res == set p | 0 <= p < j && p <= n - m && MatchAt(x, y, p)
      decreases n - m - j
    {
      // compare the window (right to left, as in the code)
      var i := m - 1;
      while i >= 0 && x[i] == y[j + i]
        invariant -1 <= i <= m - 1
        invariant forall k :: i < k < m ==> x[k] == y[j + k]
      {
        i := i - 1;
      }
      if i < 0 {
        assert MatchAt(x, y, j);
        res := res + {j};
      } else {
        assert !MatchAt(x, y, j);
      }
      var s := bc[y[j + m - 1]];
      forall p | j < p < j + s
        ensures !MatchAt(x, y, p)
      {
        HorspoolSkipSafe(x, y, bc[..], j, p);
      }
      j := j + s;
    }
  }
}
