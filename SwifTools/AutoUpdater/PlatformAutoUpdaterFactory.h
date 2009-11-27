namespace Swift {
	class AutoUpdater;
	class String;

	class PlatformAutoUpdaterFactory {
		public:
			AutoUpdater* createAutoUpdater(const String& appcastURL);
	};
}
