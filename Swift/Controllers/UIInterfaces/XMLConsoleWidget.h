#pragma once

namespace Swift {
	class XMLConsoleWidget {
		public:
			virtual ~XMLConsoleWidget();

			virtual void show() = 0;
			virtual void activate() = 0;
	};
}
