#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class StanzaEvent {
		public:
			StanzaEvent(){};
			virtual ~StanzaEvent() {};
			boost::signal<void()> onConclusion;
	};
}
