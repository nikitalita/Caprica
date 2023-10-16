#include "OSUtils.h"

#ifdef _WIN32
#include <Lmcons.h>
#include <Windows.h>
#else

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/uio.h>

#ifndef PTRACE_TRACEME
#define PTRACE_TRACEME 0 // PT_TRACE_ME
#endif
#endif
namespace caprica {

std::string getComputerName() {
#ifdef _WIN32
  char compNameBuf[MAX_COMPUTERNAME_LENGTH + 1];
  DWORD compNameBufLength = sizeof(compNameBuf);
  if (!GetComputerNameA(compNameBuf, &compNameBufLength))
    throw std::runtime_error("Failed to get the computer name!");
  return std::string(compNameBuf, compNameBufLength);
#else
  char compNameBuf[256];
  if (gethostname(compNameBuf, sizeof(compNameBuf)) != 0)
    throw std::runtime_error("Failed to get the computer name!");
  return {compNameBuf};
#endif
}

void debugBreak() {
#ifdef _WIN32
  if (IsDebuggerPresent())
    __debugbreak();
#else
  if (ptrace(PTRACE_TRACEME, 0, reinterpret_cast<caddr_t>(1), 0) == -1)
    raise(SIGTRAP);
#endif
}

std::string getUserName() {
#ifdef _WIN32
  char userNameBuf[UNLEN + 1];
  DWORD userNameBufLength = sizeof(userNameBuf);
  if (!GetUserNameA(userNameBuf, &userNameBufLength))
    throw std::runtime_error("Failed to get the user name!");
  return std::string(userNameBuf, userNameBufLength);
#else
  char userNameBuf[256];
  if (getlogin_r(userNameBuf, sizeof(userNameBuf)) != 0)
    throw std::runtime_error("Failed to get the user name!");
  return {userNameBuf};
#endif
}

int openFile(const char *path, int flags, int mode) {
#ifdef _WIN32
  return _open(path, _O_BINARY | _O_SEQUENTIAL | flags, mode);
#else
  return open(path, flags);
#endif
}

size_t readFile(int fd, void *buf, size_t count) {
#ifdef _WIN32
  return _read(fd, buf, count);
#else
  return read(fd, buf, count);
#endif
}

size_t writeFile(int fd, const void *buf, size_t count) {
#ifdef _WIN32
  return _write(fd, buf, count);
#else
  return write(fd, buf, count);
#endif
}

int isEOF(int fd, char *buf, size_t len) {
#ifdef _WIN32
  return _eof(fd);
#else
  return buf[len] == '\0';
#endif
}

int closeFile(int fd) {
#ifdef _WIN32
  return _close(fd);
#else
  return close(fd);
#endif
}

int caselessCompare(const char *a, const char *b, size_t len) {
#ifdef _WIN32
  return _strnicmp(a, b, len);
#else
  return strncasecmp(a, b, len);
#endif
}

int caselessCompare(const char *a, const char *b) {
#ifdef _WIN32
  return _stricmp(a, b);
#else
  return strcasecmp(a, b);
#endif
}

int safeitoa(int value, char *buffer, size_t size, int base) {
#ifdef _WIN32
  return _itoa_s(value, buffer, size, base);
#else
  return snprintf(buffer, size, (base == 16 ? "%x" : (base == 8 ? "%o" : "%d")), value) <= 0 ? -1 : 0;
#endif
}
}