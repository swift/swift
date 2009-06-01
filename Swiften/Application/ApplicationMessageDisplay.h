#ifndef SWIFTEN_ApplicationMessageDisplay_H
#define SWIFTEN_ApplicationMessageDisplay_H

namespace Swift {
	class String;

	class ApplicationMessageDisplay {
		public:
			virtual ~ApplicationMessageDisplay();

			virtual void setMessage(const String& message) = 0;
	};
}

#endif
