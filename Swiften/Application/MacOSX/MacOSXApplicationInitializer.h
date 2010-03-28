#ifndef SWIFTEN_MacOSXApplicationInitializer_H
#define SWIFTEN_MacOSXApplicationInitializer_H

namespace Swift {
	class MacOSXApplicationInitializer {
		public:
			MacOSXApplicationInitializer();
			~MacOSXApplicationInitializer();

		private:
			class Private;
			Private* d;
	};
}

#endif
