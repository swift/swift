#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class SessionStream {
		public:
			virtual ~SessionStream();

			virtual void writeHeader(const ProtocolHeader& header) = 0;
			virtual void writeElement(boost::shared_ptr<Element>) = 0;

			virtual bool supportsTLSEncryption() = 0;
			virtual void addTLSEncryption() = 0;

			virtual void addWhitespacePing() = 0;

			virtual void resetXMPPParser() = 0;

			boost::signal<void (const ProtocolHeader&)> onStreamStartReceived;
			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
	};
}
