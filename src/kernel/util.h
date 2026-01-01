#include <cstddef>
#include <cstdint>
#include <type_traits>

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

  void panic(const char *str);

  void panic(const char *str, const char *file, int line);

  template <typename T, size_t N>
  constexpr size_t arr_sizeof(const T(&)[N]) { return N; }

  template <typename T>
  constexpr int first_unset_bit(T val)
  {
    static_assert(std::is_integral_v<T>);
    return __builtin_ctzl(~val);
  }

  // Base of the kernel text mapping region, -2GB
  constexpr size_t KERN_OFFSET = 0xffffffff80000000;
}

extern "C" void *memset(void *buf, int c, size_t n);

#define assert(x) do { if (!(x)) { ckern::Util::panic("Assertion failed: " #x, __FILE__, __LINE__); }} while (0)

#endif
