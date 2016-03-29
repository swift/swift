/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/boost_bsignals.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>

namespace Swift {
	
	class XMLConsoleWidgetFactory;
	class XMLConsoleWidget;

	class XMLConsoleController {
		public:
			XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory);
			~XMLConsoleController();

		public:
			void handleDataRead(const SafeByteArray& data);
			void handleDataWritten(const SafeByteArray& data);

		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);

		private:
			XMLConsoleWidgetFactory* xmlConsoleWidgetFactory;
			XMLConsoleWidget* xmlConsoleWidget;
	};
}
