/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include <Swiften/Base/SafeByteArray.h>
#include <iostream>
namespace Swift {
	
	class XMLConsoleWidgetFactory;
	class XMLConsoleWidget;
	class StanzaChannel;
	class IQ;
	class Message;
	class Presence;
	class IQRouter;
	class StanzaChannelPresenceSender;
	class DirectedPresenceSender;
	class XMLConsoleController {
		public:
			XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory);
			XMLConsoleController();
			~XMLConsoleController();
		public:
			void handleDataRead(const SafeByteArray& data);
			void handleDataWritten(const SafeByteArray& data);
			void setClientStanzaChannel(StanzaChannel*, IQRouter*);
			boost::signal<void (boost::shared_ptr<Message>)> sendRawMessagePayload;
			boost::signal<void (boost::shared_ptr<IQ>)> sendRawIQPayload;
			boost::signal<void (boost::shared_ptr<Presence>)> sendRawPresencePayload;
			boost::signal<void ()> sendStatus; 

		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleUIMsgEvent(boost::shared_ptr<UIEvent> event);
		private:
			UIEventStream* uiEventStream_;
			XMLConsoleWidgetFactory* xmlConsoleWidgetFactory;
			XMLConsoleWidget* xmlConsoleWidget;
			StanzaChannel* clientStanzaChannel_;
			XMLConsoleWidget* xmlConsoleMsgWidget;
			StanzaChannelPresenceSender* stanzaChannelPresenceSender_;
			DirectedPresenceSender* stanzaChannelDirectedPresenceSender_;
			
	};
}
