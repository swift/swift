#ifndef SWIFTEN_XMPPLAYER_H
#define SWIFTEN_XMPPLAYER_H

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/noncopyable.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Parser/XMPPParserClient.h"

namespace Swift {
	class XMPPParser;
	class PayloadParserFactoryCollection;
	class XMPPSerializer;
	class PayloadSerializerCollection;

	class XMPPLayer : public XMPPParserClient, boost::noncopyable {
		public:
			XMPPLayer(
					PayloadParserFactoryCollection* payloadParserFactories,
					PayloadSerializerCollection* payloadSerializers);
			~XMPPLayer();

			void writeHeader(const String& domain);
			void writeFooter();
			void writeElement(boost::shared_ptr<Element>);
			void writeData(const String& data);

			void parseData(ByteArray data);
			void resetParser();

		public:
			boost::signal<void (const String& domain)> onStreamStart;
			boost::signal<void (boost::shared_ptr<Element>)> onElement;
			boost::signal<void (const ByteArray&)> onWriteData;
			boost::signal<void (const ByteArray&)> onDataRead;
			boost::signal<void ()> onError;

		private:
			void handleStreamStart(const String&);
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamEnd();

			void doResetParser();

		private:
			PayloadParserFactoryCollection* payloadParserFactories_;
			XMPPParser* xmppParser_;
			PayloadSerializerCollection* payloadSerializers_;
			XMPPSerializer* xmppSerializer_;
			bool resetParserAfterParse_;
			bool inParser_;
	};
}

#endif
