#ifndef SASL_PLAINMESSAGE_H
#define SASL_PLAINMESSAGE_H

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PLAINMessage
	{
		public:
			PLAINMessage(const String& authcid, const String& password, const String& authzid = "");

			const ByteArray& getValue() {
				return value_;
			}

		private:
			ByteArray value_;
	};
}

#endif
