/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_Status_H
#define SWIFTEN_Status_H

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class Status : public Payload {
		public:
			Status(const String& text = "") : text_(text) {
			}

			void setText(const String& text) {
				text_ = text;
			}

			const String& getText() const {
				return text_;
			}

		private:
			String text_;
	};
}

#endif
