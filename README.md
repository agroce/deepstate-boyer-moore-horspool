[DeepState](https://github.com/trailofbits/deepstate) harness for https://github.com/FooBarWidget/boyer-moore-horspool

You will need to clone that repo and copy all the `.cpp` files to this directory for testing to work.

The file bmhturbofail.cpp is an example of a [real bug](https://github.com/FooBarWidget/boyer-moore-horspool/issues/4) found with this harness.  But it's a hard one to find!  Days or weeks of fuzzing may fail to detect it, even with good fuzzers.

# The bug is not local to this code, but was in the posted Turbo BM implementation

The bug is not local to the FooBarWidget implementation; it's a bad shift introduced in the C implementation by the original Turbo BM authors, though not present in their pseudocode. Needle dccacdcc, haystack abcbadccdccacdcc exposes the bug nicely in [the authors' C version](http://www-igm.univ-mlv.fr/~lecroq/string/node15.html#SECTION00150) [linked to by NIST](https://xlinux.nist.gov/dads/HTML/turboBM.html) (since fixed, see below!)

See my [bug report to MariaDB](https://jira.mariadb.org/browse/MDEV-42279) for details and some history on this interesting little piece of code, which includes some reasonable fixes, though the canonical fix is the one from Lecroq, noted below and that should be online now.

# The bug is now fixed in the standard references, and passes a severe gauntlet of tests & proof effort

I received a fixed version very quickly from Thierry Lecroq, once I brought this matter to his attention, which will now be posted online and in SMART. It has passed a large number of tests and partial proof (see the verification subdirectory) and I think there is very little chance it has any problems, based only on that.  The proofs in the Dafny subdirectory of verification push that chance to fairly close to zero.  The Horspool proof is complete, the Turbo BM and new fix for SMART proofs do rely on the unproven (and much nastier proof effort, I think) linear good-suffix construction.
