#include "util.h"

void memcpy(char *dest, const char *src, int n)
{
  for (int i = 0; i < n; i++) { *(dest++) = *(src++); }
}

void memset(char *dest, int c, int n)
{
  for (int i = 0; i < n; i++) { *(dest++) = (char)c; }
}
