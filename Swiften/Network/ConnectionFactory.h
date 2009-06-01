#ifndef SWIFTEN_ConnectionFactory_H
#define SWIFTEN_ConnectionFactory_H

namespace Swift {
	class String;
	class Connection;

	class ConnectionFactory {
		public:
			virtual ~ConnectionFactory();

			virtual Connection* createConnection(const String& domain) = 0;
	};
}

#endif
