#ifndef SWIFTEN_ConnectionFactory_H
#define SWIFTEN_ConnectionFactory_H

namespace Swift {
	class Connection;

	class ConnectionFactory {
		public:
			virtual ~ConnectionFactory();

			virtual Connection* createConnection() = 0;
	};
}

#endif
