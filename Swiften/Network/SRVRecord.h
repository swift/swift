#pragma once

namespace Swift {
	struct SRVRecord {
		std::string hostname;
		int port;
		int priority;
		int weight;
	};
}
