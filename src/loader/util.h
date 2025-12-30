#ifndef LOADER_UTIL_H
#define LOADER_UTIL_H

void memcpy(char *dest, const char *src, int n)
{
  for (int i = 0; i < n; i++) { *(dest++) = *(src++); }
}

#endif
