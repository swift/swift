/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Application/Application.h"
#include "Swiften/Application/NullApplicationMessageDisplay.h"

#include <iostream>

namespace Swift {
	class UnixApplication : public Application {
		public:
			UnixApplication(const String& name) : Application(name) {
			}

			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() {
				return  &messageDisplay_;
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

