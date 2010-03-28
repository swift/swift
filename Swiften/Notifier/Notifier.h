#pragma once

#include <boost/function.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
	class Notifier {
		public:
			virtual ~Notifier();

			enum Type { ContactAvailable, ContactUnavailable, ContactStatusChange, IncomingMessage };

			/**
			 * Picture is a PNG image.
			 */
			virtual void showMessage(
				Type type,
				const String& subject, 
				const String& description, 
				const ByteArray& picture, 
				boost::function<void()> callback) = 0;
	};
}
