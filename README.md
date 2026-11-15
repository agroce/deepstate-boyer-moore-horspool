[DeepState](https://github.com/trailofbits/deepstate) harness for https://github.com/FooBarWidget/boyer-moore-horspool

You will need to clone that repo and copy all the `.cpp` files to this directory for testing to work.

The file bmhturbofail.cpp is an example of a [real bug](https://github.com/FooBarWidget/boyer-moore-horspool/issues/4) found with this harness.  But it's a hard one to find!  Days or weeks of fuzzing may fail to detect it, even with good fuzzers.

It looks as if the bug is not local to the FooBarWidget implementation; it's a bad shift introduced in the C implementation by the original Turbo BM authors, though not present in their pseudocode. Needle dccacdcc, haystack abcbadccdccacdcc exposes the bug nicely in [the authors' C version](http://www-igm.univ-mlv.fr/~lecroq/string/node15.html#SECTION00150) [linked to by NIST]([https://xlinux.nist.gov/dads/](https://xlinux.nist.gov/dads/HTML/turboBM.html)).
