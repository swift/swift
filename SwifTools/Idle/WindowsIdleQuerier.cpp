/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Idle/WindowsIdleQuerier.h>

#include <windows.h>

namespace Swift {

WindowsIdleQuerier::WindowsIdleQuerier() {
}

int WindowsIdleQuerier::getIdleTimeSeconds() {
	LASTINPUTINFO info;
	info.cbSize = sizeof(info);
	if (GetLastInputInfo(&info)) {
		return (GetTickCount() - info.dwTime) / 1000;
	}
	else {
		return 0;
	}
}

}
