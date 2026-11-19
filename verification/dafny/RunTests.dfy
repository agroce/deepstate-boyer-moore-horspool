include "TurboBM_lecroq.dfy"
import opened TurboBMLecroq
method S(s: string) returns (r: seq<int>) ensures |r| == |s| { r := []; var k := 0; while k < |s| invariant 0 <= k <= |s| && |r| == k { r := r + [(s[k] as int) % 256]; k := k + 1; } }
method Main() {
  var cases := [("dadbddad","dadbddaddadbddad"), ("dccacdcc","abcbadccdccacdcc"), ("ceebecee","aaaacceeceebecee"), ("ab","abababab")];
  var c := 0;
  while c < |cases| {
    var (p, t) := cases[c];
    var x := S(p); var y := S(t);
    if |x| >= 1 && (forall k :: 0 <= k < |x| ==> 0 <= x[k] < SIGMA) && (forall k :: 0 <= k < |y| ==> 0 <= y[k] < SIGMA) {
      var r := TurboBM(x, y);
      print p, " in ", t, ": occurrences at ", r, "\n";
    }
    c := c + 1;
  }
}
