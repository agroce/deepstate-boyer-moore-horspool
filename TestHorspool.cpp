#include <deepstate/DeepState.hpp>

using namespace deepstate;

#include <cstring>

#include "Horspool.cpp"
#include "BoyerMooreAndTurbo.cpp"

#define MAX_NEEDLE 1024
#define MAX_HAYSTACK 1024

char *
strnstr(const char *s, const char *find, size_t slen)
{
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if (slen-- < 1 || (sc = *s++) == '\0')
					return (NULL);
			} while (sc != c);
			if (len > slen)
				return (NULL);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}

TEST(Horspool, Differential) {
  size_t needle_length = DeepState_UIntInRange(1, MAX_NEEDLE);
  size_t haystack_length = DeepState_UIntInRange(1, MAX_HAYSTACK);

  LOG(TRACE) << "needle length: " << needle_length << " ; haystack length: " << haystack_length;
  
  const unsigned char* needle = (const unsigned char*)DeepState_CStr(needle_length);
  const unsigned char* haystack = (const unsigned char*)DeepState_CStr(haystack_length);
  
  for (size_t i = 0; i < needle_length; i++) {
    LOG(TRACE) << "needle[" << i << "] = " << (unsigned)needle[i];
  }

  for (size_t i = 0; i < haystack_length; i++) {
    LOG(TRACE) << "haystack[" << i << "] = " << (unsigned)haystack[i];
  }

  const occtable_type occ = CreateOccTable(needle, needle_length);
  const skiptable_type skip = CreateSkipTable(needle, needle_length);

  size_t BMH_result = (int) SearchInHorspool(haystack, haystack_length, occ, needle, needle_length);
  size_t BM_result = (int) SearchIn(haystack, haystack_length, occ, skip, needle, needle_length);
  size_t Turbo_result = (int) SearchInTurbo(haystack, haystack_length, occ, skip, needle, needle_length);
  char *strnstr_ptr = strnstr((const char*)haystack, (const char*)needle, haystack_length);
  ASSERT (BMH_result == BM_result) << "Boyer-Moore-Horspool: " <<  BMH_result << " vs. Boyer-Moore: " << BM_result;
  ASSERT (BM_result == Turbo_result) << "Boyer-Moore: " <<  BM_result << " vs. Turbo: " << Turbo_result;
  if (strnstr_ptr == NULL) {
    ASSERT (BMH_result == haystack_length) <<
      "strnstr did not find, but Boyer-Moore-Horspool returned " << BMH_result;
  } else {
    size_t strnstr_result = strnstr_ptr - (char*)haystack;
    ASSERT (strnstr_result == BMH_result) << "strnstr: " << strnstr_result << " vs. Boyer-Moore-Horspool: " << BMH_result;
  }
}
