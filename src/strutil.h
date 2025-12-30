#include <cstdint>
#include <type_traits>

#ifndef CKERN_STRUTIL_H
#define CKERN_STRUTIL_H

namespace ckern
{
  inline unsigned __itoa_len(uint32_t val, unsigned base)
  {
    unsigned n = 1;
    unsigned b = base;
    while (b <= val)
    {
      val /= base;
      n++;
    }
    return n;
  }

  inline char * itoa(int32_t val, char *buf, unsigned base)
  {
    const static char digits[16]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    int pos = 0;
    if (base == 10 && val < 0)
    {
      buf[pos] = '-';
      pos++;
      val = -val;
    }

    auto uval = static_cast<uint32_t>(val);
    auto len = __itoa_len(val, base);

    for (int i = len - 1; i >= 0; i--)
    {
      buf[pos + i] = digits[uval % base];
      uval /= base;
    }

    return buf + len;
  }

  inline int strlen(const char *s)
  {
    int len = 0;
    while (*s++ != '\0') { len++; }
    return len;
  }

  inline char *strcat(char *dest, const char *source)
  {
    const auto old_len = strlen(dest);
    dest = dest + old_len;
    while (*source) 
    { 
      *(dest++) = *(source++);
    }
    *dest = '\0';
    return dest;
  }

  inline int sprintf(char *buf, const char *fmt)
  {
    int len = 0;
    for (; *fmt != '\0'; fmt++)
    {
      *buf++ = *fmt;
      len++;
    }

    return len;
  }

  template<typename T, typename ...Args>
  inline int sprintf(char *buf, const char *fmt, T val, Args... args)
  {
    int len = 0;
    for (; *fmt != '\0'; fmt++)
    {
      if (*fmt == '%')
      {
        char *new_buf{buf};
        switch (*(++fmt))
        {
          case 'c':
            if constexpr (std::is_convertible_v<T, char>)
            {
              *(new_buf++) = static_cast<char>(val);
            }
            break;
          case 'd':
            if constexpr (std::is_convertible_v<T, int32_t>)
            {
              new_buf = itoa(static_cast<int32_t>(val), buf, 10);
            }
            break;
          case 's':
            if constexpr (std::is_convertible_v<T, const char *>)
            {
              new_buf = strcat(buf, reinterpret_cast<const char *>(val));
            }
            break;
          case 'x':
            if constexpr (std::is_convertible_v<T, int32_t>)
            {
              new_buf = itoa(static_cast<int32_t>(val), buf, 16);
            }
            break;
        }
        len += new_buf - buf;
        return len + sprintf(new_buf, fmt + 1, args...);
      }

      *buf++ = *fmt;
      len++;
    }
    return len;
  }
}

#endif
