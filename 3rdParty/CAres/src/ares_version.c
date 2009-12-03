/* $Id: ares_version.c,v 1.5 2009-11-02 11:55:53 yangtse Exp $ */

#include "ares_setup.h"
#include "ares.h"

const char *ares_version(int *version)
{
  if(version)
    *version = ARES_VERSION;

  return ARES_VERSION_STR;
}
