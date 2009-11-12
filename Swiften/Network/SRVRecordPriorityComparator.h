#pragma once

#include "Swiften/Network/SRVRecord.h"

namespace Swift {
	struct SRVRecordPriorityComparator {
		bool operator()(const SRVRecord& a, const SRVRecord& b) const {
			return a.priority < b.priority;
		}
	};
}
