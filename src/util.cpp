#include "util.h"
#include "video.h"

void ckern::Util::memcpy(void *dst, const void *src, size_t n)
{
  const char *src_c = reinterpret_cast<const char *>(src);
  char *dst_c = (char *)dst;
  for (size_t i = 0; i < n; i++)
  {
    *dst_c++ = *src_c++;
  }
}

void *ckern::Util::memset(void *buf, int c, size_t n)
{
  char *buf_c = reinterpret_cast<char *>(buf);
  for (size_t i = 0; i < n; i++)
  {
    *buf_c++ = static_cast<char>(c);
  }
  return buf;
}

void ckern::Util::panic(const char *str)
{
  ckern::Framebuffer::set_color(Framebuffer::Color::White, Framebuffer::Color::Red);
  ckern::Framebuffer::cls();
  ckern::Framebuffer::printf("PANIC: %s\nMachine halted.", str);
  __asm__ volatile("cli; hlt");
  while (1) {};
}

void ckern::Util::panic(const char *str, const char *file, int line)
{
  ckern::Framebuffer::set_color(Framebuffer::Color::White, Framebuffer::Color::Red);
  ckern::Framebuffer::cls();
  ckern::Framebuffer::printf("PANIC: %s (%s:%d)\nMachine halted.", str, file, line);
  __asm__ volatile("cli; hlt");
  while (1) {};
}

void *memset(void *buf, int c, size_t n) { return ckern::Util::memset(buf, c, n); }

extern "C" void _outb(uint16_t port, uint8_t c);
void ckern::Util::outb(uint16_t port, uint8_t c) { _outb(port, c); }

extern "C" uint8_t _inb(uint16_t port);
uint8_t ckern::Util::inb(uint16_t port) { return _inb(port); }

extern "C" uint16_t _inw(uint16_t port);
uint16_t ckern::Util::inw(uint16_t port) { return _inw(port); }

extern "C" uint32_t _indw(uint16_t port);
uint32_t ckern::Util::indw(uint16_t port) { return _indw(port); }
