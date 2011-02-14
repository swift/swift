/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <string>
#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQChannel;
	class IQHandler;

	class IQRouter {
		public:
			IQRouter(IQChannel* channel);
			~IQRouter();

			/**
			 * Sets the 'from' JID for all outgoing IQ stanzas.
			 *
			 * By default, IQRouter does not add a from to IQ stanzas, since
			 * this is automatically added by the server. This overrides this
			 * default behavior, which is necessary for e.g. components.
			 */
			void setFrom(const JID& from) {
				from_ = from;
			}

			void addHandler(IQHandler* handler);
			void removeHandler(IQHandler* handler);
			void addHandler(boost::shared_ptr<IQHandler> handler);
			void removeHandler(boost::shared_ptr<IQHandler> handler);

			/**
			 * Sends an IQ stanza.
			 *
			 * If a JID was specified using setFrom, the JID will
			 * be set as the 'from' address on iq before sending
			 * it.
			 */
			void sendIQ(boost::shared_ptr<IQ> iq);
			std::string getNewIQID();
			
			bool isAvailable();

		private:
			void handleIQ(boost::shared_ptr<IQ> iq);
			void processPendingRemoves();

		private:
			IQChannel* channel_;
			JID from_;
			std::vector< boost::shared_ptr<IQHandler> > handlers_;
			std::vector< boost::shared_ptr<IQHandler> > queuedRemoves_;
			bool queueRemoves_;
	};
}
