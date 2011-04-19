/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {
	class LastLineTracker {
		public:
			LastLineTracker();
			void setHasFocus(bool focus);
			bool getShouldMoveLastLine();
		private:
			bool lastFocus;
			bool shouldMove;
	};
}
