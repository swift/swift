/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <windows.h>

namespace Swift {
	class WindowsRegistry {
		public:
			static bool isFIPSEnabled() {
				char* pathForXP = "System\\CurrentControlSet\\Control\\Lsa";
				char* pathSinceVista = "System\\CurrentControlSet\\Control\\Lsa\\FIPSAlgorithmPolicy";
				char* keyForXP = "FIPSAlgorithmPolicy";
				char* keySinceVista = "Enabled";

				OSVERSIONINFO osvi;
				ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				GetVersionEx(&osvi);

				char* keyForOS = osvi.dwMajorVersion < 6 ? keyForXP : keySinceVista;
				char* pathForOS = osvi.dwMajorVersion < 6 ? pathForXP : pathSinceVista;

				/* http://support.microsoft.com/kb/811833 */
				/* http://msdn.microsoft.com/en-us/library/ms724911%28VS.85%29.aspx */
				HKEY key;
				bool result = false;
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							pathForOS,
							0,
							KEY_READ,
							&key) != ERROR_SUCCESS) {
					/* If we can't find the key that says we're FIPS, we're not FIPS */
					return result;
				}
				DWORD keyType = REG_DWORD;
				DWORD data;
				DWORD length = sizeof(data);

				if (RegQueryValueEx(key,
						keyForOS,
						NULL,
						&keyType,
						(LPBYTE)&data,
						&length) == ERROR_SUCCESS) {
					result = data != 0;
				}

				RegCloseKey(key);
				return result;
			}
	};
}
