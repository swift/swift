#ifndef SWIFTEN_StatusShow_H
#define SWIFTEN_StatusShow_H

#include "Swiften/Elements/Payload.h"

namespace Swift {
	class StatusShow : public Payload {
		public:
			enum Type { Online, Away, FFC, XA, DND, None };

			StatusShow(const Type& type = Online) : type_(type) {
			}

			void setType(const Type& type) {
				type_ = type;
			}

			const Type& getType() const {
				return type_;
			}

		private:
			Type type_;
	};
}

#endif
