#pragma once

namespace Swift {
	class String;

	class XMLConsoleWidget {
		public:
			virtual ~XMLConsoleWidget();

			virtual void handleDataRead(const String& data) = 0;
			virtual void handleDataWritten(const String& data) = 0;

			virtual void show() = 0;
			virtual void activate() = 0;
	};
}
