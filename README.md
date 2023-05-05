[DeepState](https://github.com/trailofbits/deepstate) harness for https://github.com/FooBarWidget/boyer-moore-horspool

You will need to clone that repo and copy all the `.cpp` files to this directory for testing to work.

The file bmhturbofail.cpp is an example of a [real bug](https://github.com/FooBarWidget/boyer-moore-horspool/issues/4) found with this harness.  But it's a hard one to find!  Hours of fuzzing may fail to detect it, even with good fuzzers.
