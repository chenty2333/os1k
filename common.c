#include "common.h"

void printf(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  while (*fmt) {
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
      case '\0':
        putchar('%');
        goto end;
      case '%':
        putchar('%');
        break;
      case 's': {
        const char *s = va_arg(vargs, const char *);
        while (*s) {
          putchar(*s);
          s++;
        }
        break;
      }
      case 'd': {
        int value = va_arg(vargs, int);
        unsigned magnitude = value;
        if (value < 0) {
          putchar('-');
          magnitude = -magnitude;
        }

        unsigned divisor = 1;
        while (magnitude / divisor > 9)
          divisor *= 10;

        while (divisor > 0) {
          putchar('0' + magnitude / divisor);
          magnitude %= divisor;
          divisor /= 10;
        }

        break;
      }
      case 'x': {
        unsigned value = va_arg(vargs, unsigned);
        for (int i = 7; i >= 0; i--) {

          unsigned nibble = (value >> (i * 4)) & 0xf;
          putchar("0123456789abcdef"[nibble]);
        }
      }
      }
    } else {
      putchar(*fmt);
    }

    fmt++;
  }

end:
  va_end(vargs);
}

void *memcpy(void *dst, const void *src, size_t n) {
  uint8_t *d = (uint8_t *)dst;
  const uint8_t *s = (const uint8_t *)src;
  while (n--)
    *d++ = *s++;
  return dst;
}

void *memcpy_s(void *dst, const void *src, size_t n) {
  if (dst == NULL || src == NULL)
    return dst;

  uint8_t *d = (uint8_t *)dst;
  const uint8_t *s = (const uint8_t *)src;

  while (n && ((paddr_t)d & (sizeof(size_t) - 1))) {
    *d++ = *s++;
    n--;
  }

  paddr_t s_addr = (paddr_t)s;
  size_t *dw = (size_t *)d;

  if ((s_addr & (sizeof(size_t) - 1)) == 0) {
    const size_t *sw = (const size_t *)s;
    while (n >= sizeof(size_t)) {
      *dw++ = *sw++;
      n -= sizeof(size_t);
    }
    s = (const uint8_t *)sw;
  } else {

    int offest = s_addr & (sizeof(size_t) - 1);
    int shift_right = offest * 8;
    int shift_left = (sizeof(size_t) * 8) - shift_right;

    const size_t *sw = (const size_t *)(s_addr & ~(sizeof(size_t) - 1));

    size_t w1 = *sw++;

    while (n >= sizeof(size_t)) {
      size_t w2 = *sw++;

      size_t result = (w1 >> shift_right) | (w2 << shift_left);

      *dw++ = result;

      w1 = w2;
      n -= sizeof(size_t);
    }

    s = (const uint8_t *)s + ((uint8_t *)dw - d);
  }

  d = (uint8_t *)dw;

  while (n--)
    *d++ = *s++;

  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
  if (dst < src) {
    return memcpy_s(dst, src, n);
  } else {
    uint8_t *d = (uint8_t *)dst + n;
    const uint8_t *s = (const uint8_t *)src + n;
    while (n--)
      *--d = *--s;
    return dst;
  }
}

void *memset(void *buf, char c, size_t n) {
  uint8_t *p = (uint8_t *)buf;
  while (n--)
    *p++ = c;
  return buf;
}

char *strcpy(char *dst, const char *src) {
  char *d = dst;
  while (*src)
    *d++ = *src++;
  *d = '\0';
  return dst;
}

char *strcpy_s(char *dst, const char *src, size_t n) {
  if (dst == NULL || src == NULL)
    return dst;

  char *d = dst;
  while (n && *src) {
    *d++ = *src++;
    n--;
  }

  while (n--)
    *d++ = '\0';

  return dst;
}

size_t strlcpy(char *dst, const char *src, size_t size) {
  size_t src_len = 0;

  while (src[src_len])
    src_len++;

  if (size > 0) {
    size_t copy_len = (src_len < size - 1) ? src_len : size - 1;
    memcpy_s(dst, src, copy_len);
    dst[copy_len] = '\0';
  }

  return src_len;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2)
      break;
    s1++;
    s2++;
  }

  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n > 0 && *s1 && *s2) {
    if (*s1 != *s2)
      break;
    s1++;
    s2++;
    n--;
  }

  if (n == 0)
    return 0;

  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s++)
    len++;
  return len;
}

char *strchr(const char *s, int c) {
  while (*s) {
    if (*s == c)
      return (char *)s;
    s++;
  }
  return NULL;
}
