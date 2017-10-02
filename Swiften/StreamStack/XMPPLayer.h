/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/Parser/XMPPParserClient.h>
#include <Swiften/StreamStack/HighLayer.h>

namespace Swift {
    class ProtocolHeader;
    class XMPPParser;
    class PayloadParserFactoryCollection;
    class XMPPSerializer;
    class PayloadSerializerCollection;
    class XMLParserFactory;
    class BOSHSessionStream;

    class SWIFTEN_API XMPPLayer : public XMPPParserClient, public HighLayer, boost::noncopyable {
        friend class BOSHSessionStream;
        public:
            XMPPLayer(
                    PayloadParserFactoryCollection* payloadParserFactories,
                    PayloadSerializerCollection* payloadSerializers,
                    XMLParserFactory* xmlParserFactory,
                    StreamType streamType,
                    bool setExplictNSonTopLevelElements = false);
            virtual ~XMPPLayer();

            void writeHeader(const ProtocolHeader& header);
            void writeFooter();
            void writeElement(std::shared_ptr<ToplevelElement>);
            void writeData(const std::string& data);

            void resetParser();

        protected:
            void handleDataRead(const SafeByteArray& data);
            void writeDataInternal(const SafeByteArray& data);

        public:
            boost::signals2::signal<void (const ProtocolHeader&)> onStreamStart;
            boost::signals2::signal<void ()> onStreamEnd;
            boost::signals2::signal<void (std::shared_ptr<ToplevelElement>)> onElement;
            boost::signals2::signal<void (const SafeByteArray&)> onWriteData;
            boost::signals2::signal<void (const SafeByteArray&)> onDataRead;
            boost::signals2::signal<void ()> onError;

        private:
            void handleStreamStart(const ProtocolHeader&);
            void handleElement(std::shared_ptr<ToplevelElement>);
            void handleStreamEnd();

            void doResetParser();

        private:
            PayloadParserFactoryCollection* payloadParserFactories_;
            XMPPParser* xmppParser_;
            PayloadSerializerCollection* payloadSerializers_;
            XMLParserFactory* xmlParserFactory_;
            XMPPSerializer* xmppSerializer_;
            bool setExplictNSonTopLevelElements_;
            bool resetParserAfterParse_;
            bool inParser_;
    };
}
