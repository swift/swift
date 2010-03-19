#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class Event {
		public:
			Event(){};
			virtual ~Event() {};
			boost::signal<void()> onConclusion;
	};
}
