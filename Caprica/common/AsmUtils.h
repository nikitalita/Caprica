#pragma once
#if defined(__arm__) || defined(_M_ARM) || defined(__aarch64__) || defined(_M_ARM64)
#ifndef __ARM_NEON__
#error "ARM target without NEON support!"
#endif

#include <common/sse2neon.h>

#else
#include <intrin.h>
#endif

