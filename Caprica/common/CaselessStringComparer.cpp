#include <common/CaselessStringComparer.h>

#include <common/AsmUtils.h>
namespace caprica {

alignas(128) const uint64_t charToLowerMap[] = { ' ', '!', '"', '#', '$',  '%', '&', '\'', '(', ')', '*', '+', ',', '-',
                                                 '.', '/', '0', '1', '2',  '3', '4', '5',  '6', '7', '8', '9', ':', ';',
                                                 '<', '=', '>', '?', '@',  'a', 'b', 'c',  'd', 'e', 'f', 'g', 'h', 'i',
                                                 'j', 'k', 'l', 'm', 'n',  'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w',
                                                 'x', 'y', 'z', '[', '\\', ']', '^', '_',  '`', 'a', 'b', 'c', 'd', 'e',
                                                 'f', 'g', 'h', 'i', 'j',  'k', 'l', 'm',  'n', 'o', 'p', 'q', 'r', 's',
                                                 't', 'u', 'v', 'w', 'x',  'y', 'z', '{',  '|', '}', '~' };


ALWAYS_INLINE int caselessCompare(const char *a, const char *b, size_t len) {
#ifdef _WIN32
  return _strnicmp(a, b, len);
#else
  return strncasecmp(a, b, len);
#endif
}

ALWAYS_INLINE int caselessCompare(const char *a, const char *b) {
#ifdef _WIN32
  return _stricmp(a, b);
#else
  return strcasecmp(a, b);
#endif
}

void identifierToLower(char* data, size_t size) {
  for (size_t i = 0; i < size; i++)
    *data = (char)(charToLowerMap - 0x20)[*data];
}

void identifierToLower(std::string& str) {
  identifierToLower(str.data(), str.size());
}

bool pathEq(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) {
    return false;
  }
  return caselessCompare(a.data(), b.data(), a.size()) == 0;
}

bool pathEq(std::string_view a, const char* b) {
  if (a.size() != strlen(b)) {
    return false;
  }
  return caselessCompare(a.data(), b, a.size()) == 0;
}

bool pathEq(const char *a, const char *b) {
  if (strlen(a) != strlen(b)) {
    return false;
  }
  return caselessCompare(a, b, strlen(a)) == 0;
}

bool pathEq(std::string_view a, const identifier_ref& b) {
  if (a.size() != b.size()) {
    return false;
  }
  return caselessCompare(a.data(), b.data(), a.size()) == 0;
}

bool pathEq(const identifier_ref& a, const identifier_ref& b) {
  if (a.size() != b.size()) {
    return false;
  }
  return caselessCompare(a.data(), b.data(), a.size()) == 0;
}

ALWAYS_INLINE bool CaselessIdentifierEqual::equal(const char* a, const char* b, size_t len) {
  const char* __restrict strA = a;
  const int64_t strBOff = b - strA;
  while (len) {
    if ((charToLowerMap - 0x20)[*strA] != (charToLowerMap - 0x20)[*(strA + strBOff)])
      return false;
    strA++;
    len--;
  }
  return true;

}

static bool idEq(const char* a, size_t sA, const char* b, size_t sB) {
  if (sA != sB)
    return false;
  return CaselessIdentifierEqual::equal(a, b, sB);
}

bool idEq(const char* a, const char* b) {
  return idEq(a, strlen(a), b, strlen(b));
}
bool idEq(const char* a, const std::string& b) {
  return idEq(a, strlen(a), b.c_str(), b.size());
}
bool idEq(const std::string& a, const char* b) {
  return idEq(a.c_str(), a.size(), b, strlen(b));
}
NEVER_INLINE
bool idEq(const std::string& a, const std::string& b) {
  return idEq(a.c_str(), a.size(), b.c_str(), b.size());
}
NEVER_INLINE
bool idEq(std::string_view a, std::string_view b) {
  return idEq(a.data(), a.size(), b.data(), b.size());
}

bool caselessEq(const char *a, const char *b) {
  return idEq(a, strlen(a), b, strlen(b));
}

bool caselessEq(const char *a, const std::string &b) {
  return idEq(a, strlen(a), b.c_str(), b.size());
}

bool caselessEq(const std::string &a, const char *b) {
  return idEq(a.c_str(), a.size(), b, strlen(b));
}

NEVER_INLINE
bool caselessEq(const std::string &a, const std::string &b) {
  return idEq(a.c_str(), a.size(), b.c_str(), b.size());
}

NEVER_INLINE
bool caselessEq(std::string_view a, std::string_view b) {
  return idEq(a.data(), a.size(), b.data(), b.size());
}




NEVER_INLINE
size_t CaselessStringHasher::doCaselessHash(const char* k, size_t len) {
  // Using FNV-1a hash, the same as the MSVC std lib hash of strings.
  static_assert(sizeof(size_t) == 8, "This is 64-bit only!");
  constexpr size_t offsetBasis = 0xcbf29ce484222325ULL;
  constexpr size_t prime = 0x100000001B3ULL;
  const char* cStr = k;
  const char* eStr = k + len;

  size_t val = offsetBasis;
#if 0
  for (; cStr < eStr; cStr++) {
    // This is safe only because we are hashing
    // identifiers with a known set of characters.
    val ^= (size_t)(*cStr | 32);
    val *= prime;
  }
#else
  for (; cStr < eStr; cStr++) {
    val ^= (size_t)(charToLowerMap - 0x20)[*cStr];
    val *= prime;
  }
#endif
  return val;
}

size_t CaselessPathHasher::doPathHash(const char* k, size_t len) {
  // TODO: Ensure in ascii.
  return CaselessStringHasher::doCaselessHash(k, len);
}

template <bool isNullTerminated>
ALWAYS_INLINE uint32_t CaselessIdentifierHasher::hash(const char* s, size_t len) {
  const char* cStr = s;

  size_t lenLeft = len;
  if (isNullTerminated) {
    // We know the string is null-terminated, so we can align to 2.
    lenLeft = ((len + 1) & 0xFFFFFFFFFFFFFFFEULL);
  }
  size_t iterCount = lenLeft >> 2;
  uint32_t val = 0x84222325U;
  size_t i = iterCount;
  while (i)
    val = _mm_crc32_u32(val, ((uint32_t*)cStr)[--i] | 0x20202020);
  if (lenLeft & 2) {
    val = _mm_crc32_u16(val, *(uint16_t*)(cStr + (iterCount * 4)) | (uint16_t)0x2020);
    if (!isNullTerminated) {
      // This is duplicated like this because it ends up cost-free when compared to
      // any other methods.
      if (lenLeft & 1)
        val = _mm_crc32_u8(val, *(uint8_t*)(cStr + (iterCount * 4) + 2) | (uint8_t)0x20);
    }
  } else if (!isNullTerminated) {
    if (lenLeft & 1)
      val = _mm_crc32_u8(val, *(uint8_t*)(cStr + (iterCount * 4)) | (uint8_t)0x20);
  }
  return val;
}

template uint32_t CaselessIdentifierHasher::hash<true>(const char*, size_t);
template uint32_t CaselessIdentifierHasher::hash<false>(const char*, size_t);

NEVER_INLINE
size_t CaselessIdentifierHasher::operator()(const std::string& k) const {
  auto r = CaselessIdentifierHasher::hash<false>(k.c_str(), k.size());
  return ((size_t)r << 32) | r;
}

NEVER_INLINE
size_t CaselessIdentifierHasher::operator()(std::string_view k) const {
  auto r = CaselessIdentifierHasher::hash<false>(k.data(), k.size());
  return ((size_t)r << 32) | r;
}

}
