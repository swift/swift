#ifndef SWIFTEN_MessageEvent_H
#define SWIFTEN_MessageEvent_H

#include <cassert>

#include "Swiften/Elements/Message.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class MessageEvent {
			public:
			MessageEvent(boost::shared_ptr<Message> stanza) : stanza_(stanza){}
			boost::shared_ptr<Message> getStanza() {return stanza_;}
			boost::signal<void()> onRead;

			bool isReadable() {
				return getStanza()->isError() || !getStanza()->getBody().isEmpty();
			}

			void read() {
				assert (isReadable());
				onRead();
			}

		private:
			boost::shared_ptr<Message> stanza_;
	};
}

#endif
