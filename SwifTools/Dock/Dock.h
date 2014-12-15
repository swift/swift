/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
	

	class Dock {
		public:
			virtual ~Dock();

			virtual void setNumberOfPendingMessages(int i) = 0;
	};
}
