#include "ares_setup.h"

/* $Id: windows_port.c,v 1.22 2009-11-02 11:55:54 yangtse Exp $ */

/* only do the following on windows
 */
#if (defined(WIN32) || defined(WATT32)) && !defined(MSDOS)

#ifdef __WATCOMC__
/*
 * Watcom needs a DllMain() in order to initialise the clib startup code.
 */
BOOL
WINAPI DllMain (HINSTANCE hnd, DWORD reason, LPVOID reserved)
{
  (void) hnd;
  (void) reason;
  (void) reserved;
  return (TRUE);
}
#endif

#endif /* WIN32 builds only */
