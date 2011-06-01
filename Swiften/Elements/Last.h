/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class Last : public Payload {
		public:
			Last() {};
			Last(int seconds) : seconds_(seconds) {};

			int getSeconds() const {return seconds_;}
			void setSeconds(int seconds) {seconds_ = seconds;}

		private:
			int seconds_;
	};
}
