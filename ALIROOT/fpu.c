#ifdef __linux
#include <libxml/xmlversion.h>
#if LIBXML_VERSION < 20913
#define _GNU_SOURCE 1
#include <fenv.h>
static void __attribute__ ((constructor)) trapfpe(void)
{
  /* Enable some exceptions. At startup all exceptions are masked. */
  feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
}
#else
void trapfpe () {}
#endif
#else
void trapfpe () {}
#endif
