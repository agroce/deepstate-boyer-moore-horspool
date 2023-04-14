FILES=testhorspool testhorspool_san testhorspool_LF testhorspool_AFL

all: $(FILES)

clean:
	rm -rf $(FILES)

testhorspool: TestHorspool.cpp
	clang++ -o testhorspool TestHorspool.cpp -ldeepstate

testhorspool_san: TestHorspool.cpp
	clang++ -o testhorspool_san TestHorspool.cpp -ldeepstate -fsanitize=address,undefined

testhorspool_LF: TestHorspool.cpp
	clang++ -o testhorspool_LF TestHorspool.cpp -ldeepstate_LF -fsanitize=fuzzer

testhorspool_AFL: TestHorspool.cpp
	${AFL_HOME}/afl-clang++ -o testhorspool_AFL TestHorspool.cpp -ldeepstate_AFL
