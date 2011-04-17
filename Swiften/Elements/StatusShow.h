/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class StatusShow : public Payload {
		public:
			enum Type { Online, Away, FFC, XA, DND, None };

			StatusShow(const Type& type = Online);

			void setType(const Type& type) {
				type_ = type;
			}

			const Type& getType() const {
				return type_;
			}

			/**
			 * Can be used for rough ordering of Types.
			 * Greater magnitude = more available.
			 */
			static int typeToAvailabilityOrdering(Type type) {
				switch (type) {
					case Online: return 4;
					case FFC: return 5;
					case Away: return 2;
					case XA: return 1;
					case DND: return 3;
					case None: return 0;
				}
				return 0;
			}

		private:
			Type type_;
	};
}
