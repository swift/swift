#ifndef SWIFTEN_AuthRequest_H
#define SWIFTEN_AuthRequest_H

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class AuthRequest : public Element {
		public:
			AuthRequest(const String& mechanism = "", const ByteArray& message = "") : 
					mechanism_(mechanism), message_(message) {
			}

			const ByteArray& getMessage() const {
				return message_;
			}

			void setMessage(const ByteArray& message) {
				message_ = message;
			}

			const String& getMechanism() const {
				return mechanism_;
			}

			void setMechanism(const String& mechanism) {
				mechanism_ = mechanism;
			}

		private:
			String mechanism_;
			ByteArray message_;
	};
}

#endif
