/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/optional.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Network/DomainNameResolveError.h>

namespace Swift {
	class DomainNameServiceQuery {
		public:
			typedef boost::shared_ptr<DomainNameServiceQuery> ref;

			struct Result {
				Result(const std::string& hostname = "", int port = -1, int priority = -1, int weight = -1) : hostname(hostname), port(port), priority(priority), weight(weight) {}
				std::string hostname;
				int port;
				int priority;
				int weight;
			};

			struct ResultPriorityComparator {
				bool operator()(const DomainNameServiceQuery::Result& a, const DomainNameServiceQuery::Result& b) const {
					return a.priority < b.priority;
				}
			};

			virtual ~DomainNameServiceQuery();

			virtual void run() = 0;

			boost::signal<void (const std::vector<Result>&)> onResult;
	};
}
