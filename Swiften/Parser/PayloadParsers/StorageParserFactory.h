#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StorageParser.h"

namespace Swift {
	class StorageParserFactory : public GenericPayloadParserFactory<StorageParser> {
		public:
			StorageParserFactory() : GenericPayloadParserFactory<StorageParser>("storage", "storage:bookmarks") {}
	};
}
