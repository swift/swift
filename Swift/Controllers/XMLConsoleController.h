/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include <Swiften/Base/SafeByteArray.h>

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
