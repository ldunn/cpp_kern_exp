#include <cstdint>

#ifndef CKERN_UTIL_H
#define CKERN_UTIL_H

namespace ckern::Util
{

  // Copy a block of memory
  void memcpy(void *dst, const void *src, int n);

  // Write a byte to the specified I/o port
  void outb(uint16_t port, uint8_t data);
  
  // Receive a byte from the specified I/O port
  uint8_t inb(uint16_t port);

  // Receive a word from the specified I/O port
  uint16_t inw(uint16_t port);

  // Receive a doubleword from the specified I/O port
  uint32_t indw(uint16_t port);
}

#endif
