#include <cstddef>
#include <cstdint>

#ifndef CKERN_UTIL_H
#define CKERN_UTIL_H

namespace ckern::Util
{

  // Copy a block of memory
  void memcpy(void *dst, const void *src, size_t n);

  void *memset(void *buf, int c, size_t n);

  // Write a byte to the specified I/o port
  void outb(uint16_t port, uint8_t data);
  
  // Receive a byte from the specified I/O port
  uint8_t inb(uint16_t port);

  // Receive a word from the specified I/O port
  uint16_t inw(uint16_t port);

  // Receive a doubleword from the specified I/O port
  uint32_t indw(uint16_t port);

  template <typename T, size_t N>
  constexpr size_t arr_sizeof(const T(&)[N]) { return N; }
}

extern "C" void *memset(void *buf, int c, size_t n);

#endif
