#ifndef SWIFTEN_LoginWindowFactory_H
#define SWIFTEN_LoginWindowFactory_H



namespace Swift {
	class LoginWindow;
	class String;
	class UIEventStream;
		
	class LoginWindowFactory {
		public:
			virtual ~LoginWindowFactory() {};

			/**
			 * Transfers ownership of result.
			 */
			virtual LoginWindow* createLoginWindow(UIEventStream* uiEventStream) = 0;

	};
}
#endif

