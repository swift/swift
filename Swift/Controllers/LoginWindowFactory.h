#ifndef SWIFTEN_LoginWindowFactory_H
#define SWIFTEN_LoginWindowFactory_H



namespace Swift {
	class LoginWindow;
    class String;
    
	class LoginWindowFactory {
		public:
			virtual ~LoginWindowFactory() {};

			/**
			 * Transfers ownership of result.
			 */
			virtual LoginWindow* createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) = 0;

	};
}
#endif

