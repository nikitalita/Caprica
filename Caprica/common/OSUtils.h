#pragma once

#include <string>
#ifdef _WIN32
#include <Lmcons.h>
#include <Windows.h>
#else
#include <signal.h>
#include <unistd.h>
#endif
namespace caprica {
inline void debugBreak() {
#ifdef _WIN32
  if (IsDebuggerPresent())
    __debugbreak();
#else
  if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
    raise(SIGTRAP);
#endif
}

inline std::string getComputerName() {
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
  return std::string(compNameBuf);
#endif
}

inline std::string getUserName() {
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
  return std::string(userNameBuf);
#endif
}
}