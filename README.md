[DeepState](https://github.com/trailofbits/deepstate) harness for https://github.com/FooBarWidget/boyer-moore-horspool

You will need to clone that repo and copy all the `.cpp` files to this directory for testing to work.

The file bmhturbofail.cpp is an example of a [real bug](https://github.com/FooBarWidget/boyer-moore-horspool/issues/4) found with this harness.  But it's a hard one to find!  Days or weeks of fuzzing may fail to detect it, even with good fuzzers.

# The bug is not local to this code, but was in the posted Turbo BM implementation

The bug is not local to the FooBarWidget implementation; it's a bad shift introduced in the C implementation by the original Turbo BM authors, though not in the version in Crochemore, Hancart and Lecroq's *Algorithms on Strings*, and apparently not in the 1994 paper. Needle dccacdcc, haystack abcbadccdccacdcc exposes the bug nicely in [the authors' C version](http://www-igm.univ-mlv.fr/~lecroq/string/node15.html#SECTION00150) [linked to by NIST](https://xlinux.nist.gov/dads/HTML/turboBM.html) (a fix is on its way, see below)

See my [bug report to MariaDB](https://jira.mariadb.org/browse/MDEV-42279) for details and some history on this interesting little piece of code, which includes some reasonable fixes, though the canonical fix is the one from Lecroq, noted below and that should be online now.

# The bug is being fixed in the standard references, and the fix passes a severe gauntlet of tests & proof effort

I received a fixed version very quickly from Thierry Lecroq once I brought this to his attention; it will be posted online and in SMART. It has passed a large number of tests and bounded model checking with CBMC (see `verification/`). The Dafny proofs in `verification/dafny` make a problem very unlikely: the Horspool proof is complete, and the Turbo-BM proofs (the floor removed, and Lecroq's fix) are complete for the search loop with a verified, simple good-suffix table. What remains unproved is that the linear-time good-suffix construction used in the C code computes that same table, a much nastier proof that is so far backed only by exhaustive testing on small patterns and CBMC.
