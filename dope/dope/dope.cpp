#include "dope.h"
#ifdef HAVE_CONFIG_H
#include <dopeconfig.h>
#endif

#ifdef HAVE_EXECINFO_H

#include <execinfo.h>
#include <stdlib.h>

void 
dope_backtrace()
{
#define MAXSTACK 200
  void *array[MAXSTACK];
  size_t size;
  char **strings;
  size_t i;
  
  size = backtrace (array, MAXSTACK);
  strings = backtrace_symbols (array, size);
  
  std::cerr << "Obtained "<<size<<" stack frames.\n";
  
  for (i = 0; i < size; i++)
    std::cerr << strings[i] << std::endl;
  
  free (strings);
#undef MAXSTACK
}
#else
void 
dope_backtrace()
{
}
#endif


int dope_micro_version=DOPE_MICRO_VERSION;
int dope_minor_version=DOPE_MINOR_VERSION;
int dope_major_version=DOPE_MAJOR_VERSION;
