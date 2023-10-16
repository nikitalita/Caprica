#pragma once

#include <string>
namespace caprica {
void debugBreak();

std::string getComputerName();

std::string getUserName();

int openFile(const char *path, int flags, int mode = 0);

size_t readFile(int fd, void *buf, size_t count);

size_t writeFile(int fd, const void *buf, size_t count);

int isEOF(int fd, char *buf, size_t len);

int closeFile(int fd);

int caselessCompare(const char *a, const char *b, size_t len);

int caselessCompare(const char *a, const char *b);

int safeitoa(int value, char *buffer, size_t size, int base);
}