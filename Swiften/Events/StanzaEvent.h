#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class StanzaEvent {
		public:
			StanzaEvent(){concluded_ = false;};
			virtual ~StanzaEvent() {};
			void conclude() {concluded_ = true; onConclusion();};
			/** Do not call this directly from outside the class */
			boost::signal<void()> onConclusion;
			bool getConcluded() {return concluded_;};
		private:
			bool concluded_;
	};
}
